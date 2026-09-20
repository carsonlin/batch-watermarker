#pragma once

#include "watermark.h"

#include <QImage>
#include <QMainWindow>

class QComboBox;
class QLabel;
class QLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setImage(const QImage &image);

private:
    void updatePreview();

    QLabel *m_label = nullptr;
    QLineEdit *m_textEdit = nullptr;
    QComboBox *m_hAlignBox = nullptr;
    QComboBox *m_vAlignBox = nullptr;

    QImage m_original;
    WatermarkSettings m_settings;
};