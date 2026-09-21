#ifndef JOBLISTMODEL_H
#define JOBLISTMODEL_H

#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QStringList>


enum class JobStatus{
    Queued,
    Processing,
    Done,
    Failed
};

struct Job{
    QString path;
    JobStatus status = JobStatus::Queued;
};

class JobListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit JobListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void addFiles(const QStringList &paths);
    QString pathAt(int row) const;
    void setStatus(int row, JobStatus status);
    void removeAt(int row);
    void clear();

private:
    QList<Job> m_jobs;
};

#endif // JOBLISTMODEL_H
