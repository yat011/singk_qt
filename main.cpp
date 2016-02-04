#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "message.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*configuration*/
    qRegisterMetaTypeStreamOperators<MyPair>("MyPair");
    MainWindow w;
    w.show();


    return a.exec();
}
