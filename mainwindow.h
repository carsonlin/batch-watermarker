#pragma once

#include "watermark.h"
#include <QImage>
#include <QMainWindow>
#include "joblistmodel.h"
#include "QSlider.h"
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QProgressBar;
class QTableView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setImage(const QImage &image);   // takes the ORIGINAL, unwatermarked image

private:
    void onAddImages();                   // slot for the "Add Images" button
    void updatePreview();
    void onSelectionChanged(const QModelIndex &current);
    void onRemove();
    void onClear();
    void clearPreview();
    void showRow(int row);
    void onBrowseFolder();

    QLabel *m_label = nullptr;            // the preview
    QLineEdit *m_textEdit = nullptr;
    QComboBox *m_hAlignBox = nullptr;
    QComboBox *m_vAlignBox = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_removeButton = nullptr;
    QPushButton *m_clearButton = nullptr;
    QLabel *m_outputLabel = nullptr;
    QString m_outputFolder;
    QPushButton *m_browseButton = nullptr;
    QPushButton *m_processButton = nullptr;
    QProgressBar *m_progressBar = nullptr;

    JobListModel *m_model = nullptr;
    QTableView *m_table = nullptr;

    QImage m_original;
    WatermarkSettings m_settings;
    QSlider * m_opacitySlider = nullptr;
    QSlider * m_sizeSlider = nullptr;
    QSlider * m_marginSlider = nullptr;



};