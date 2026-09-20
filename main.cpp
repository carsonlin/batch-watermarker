#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QImage>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QImage img("C:/Users/carso/OneDrive/Desktop/pblack.jpg");
    if (img.isNull())
        qDebug() << "Could not load the image. Check the path.";

    w.setImage(img);
    w.show();
    return a.exec();
}