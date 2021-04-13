#include <QCoreApplication>
#include "backend.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Backend b(&a);
    return a.exec();
}
