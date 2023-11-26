
#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon("pixelated.png"));
    w.setWindowTitle("Basic Raster Algorithms");
    w.show();
    return a.exec();
}
