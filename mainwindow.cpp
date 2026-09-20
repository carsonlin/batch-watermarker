#include "mainwindow.h"

#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_label(new QLabel(this))
{
    m_label->setAlignment(Qt::AlignCenter);
    setCentralWidget(m_label);
}

void MainWindow::showImage(const QImage &image)
{
    const QPixmap pixmap = QPixmap::fromImage(image);
    m_label->setPixmap(pixmap.scaled(800, 600, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
}