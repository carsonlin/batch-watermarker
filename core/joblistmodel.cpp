#include "joblistmodel.h"

#include <QFileInfo>

JobListModel::JobListModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int JobListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_jobs.size();
}

int JobListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant JobListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() < 0 || index.row() >= m_jobs.size() || role != Qt::DisplayRole){
        return QVariant();
    }
    const auto &job = m_jobs[index.row()];

    if(index.column() == 0){
        return QFileInfo(m_jobs[index.row()].path).fileName();
    }
    if(index.column() == 1){
        switch (job.status) {
        case JobStatus::Queued:     return "Queued";
        case JobStatus::Processing: return "Processing";
        case JobStatus::Done:       return "Done";
        case JobStatus::Failed:     return "Failed";
        }
    }
    return QVariant();
}

QVariant JobListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole || orientation != Qt::Horizontal){
        return QVariant();
    }
    if(section == 0){
        return "File";
    }
    else if(section == 1){
        return "Status";
    }
    return QVariant();
}

void JobListModel::addFiles(const QStringList &paths)
{
    if(paths.empty()){
        return;
    }
    int first = m_jobs.size();
    int last = first + paths.length() - 1;

    beginInsertRows(QModelIndex(), first, last);
    for(const QString &path : paths){
        m_jobs.append({path, JobStatus::Queued});
    }
    endInsertRows();
}

QString JobListModel::pathAt(int row) const
{
    if(row < 0 || row >= m_jobs.size()){
        return QString();
    }
    return m_jobs[row].path;
}

void JobListModel::setStatus(int row, JobStatus status)
{
    if(row < 0 || row >= m_jobs.size()){
        return;
    }
    m_jobs[row].status = status;
    emit dataChanged(index(row, 1), index(row, 1));
}

void JobListModel::removeAt(int row){
    if(row < 0 || row >= m_jobs.size()){
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    m_jobs.removeAt(row);
    endRemoveRows();
}


void JobListModel::clear(){
    beginResetModel();

    m_jobs.clear();

    endResetModel();
}