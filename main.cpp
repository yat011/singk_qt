#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "message.h"
#include "user.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*configuration*/
    qRegisterMetaTypeStreamOperators<MyPair>("MyPair");
    qRegisterMetaTypeStreamOperators<User>("User");
    qRegisterMetaType<QList<User> >("UserList");
    qRegisterMetaTypeStreamOperators<QList<User> > ("UserList");


    MainWindow w;
    w.show();


    return a.exec();
}
