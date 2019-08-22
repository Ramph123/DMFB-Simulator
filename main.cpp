#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <ctime>

int waterDrop::idMax(0);

int main(int argc, char *argv[])
{
    srand((int)time(nullptr));
    //Q_INIT_RESOURCE(MainWindow);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.installEventFilter(&w);

//    QString applicationDirPath;
//    applicationDirPath = QCoreApplication::applicationDirPath();
//    qDebug()<<"applicationDirPath: "<<applicationDirPath;

    return a.exec();
}
