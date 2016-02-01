#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.video->netController->startServer(1234);
    MainWindow w2;
    w2.show();
    w2.video->netController->connectToHost("localhost",1234);
    MainWindow w3;
    w3.show();
    w3.video->netController->connectToHost("localhost",1234);
    return a.exec();
}
