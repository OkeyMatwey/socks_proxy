#include "mainwindow.h"
#include "worker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Worker wr;
//    wr.start();
    MainWindow w;
    w.show();
    return a.exec();
}
