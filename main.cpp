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

    /*
   // w.video->addVideo("https://www.youtube.com/watch?v=KWrT-xS6OCk");
    w.video->netController->startServer(1234);
    MainWindow w2;
    w2.show();
    w2.video->netController->connectToHost("localhost",1234);
    MainWindow w3;
    w3.show();
    w3.video->netController->connectToHost("localhost",1234);
*/

    /*
    qRegisterMetaTypeStreamOperators<MyPair>("MyPair");
    Message msg, msg2;
    QByteArray byteArray;
     QDataStream out(&byteArray, QIODevice::WriteOnly);
     msg.setClientId(3);
     QMap<int,QPair<QString,QString>> map;
     map[2] = QPair<QString,QString>("hi","on0");
     msg.setLinks(map);
     out << msg;
     QDataStream in(byteArray);
     in >> msg2;

    qDebug() << msg2.clientId;
    qDebug() << msg2.getLinks();
    */
    return a.exec();
}
