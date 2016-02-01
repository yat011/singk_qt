#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "message.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*MainWindow w;
    w.show();
    w.video->netController->startServer(1234);
    MainWindow w2;
    w2.show();
    w2.video->netController->connectToHost("localhost",1234);
    MainWindow w3;
    w3.show();
    w3.video->netController->connectToHost("localhost",1234);
    */
    Message msg, msg2;
    QByteArray byteArray;
     QDataStream out(&byteArray, QIODevice::WriteOnly);
     msg.setClientId(3);
     out << msg;
     QDataStream in(byteArray);
     in >> msg2;
    qDebug() << msg2.clientId;
    return a.exec();
}
