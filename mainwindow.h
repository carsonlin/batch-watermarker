#pragma once

#include <QImage>
#include <QMainWindow>

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void showImage(const QImage &image);

private:
    QLabel *m_label;
};