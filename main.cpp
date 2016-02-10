#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QQmlApplicationEngine>
#include "message.h"
#include "user.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QGuiApplication app(argc, argv);
    /*configuration*/
    qRegisterMetaTypeStreamOperators<MyPair>("MyPair");
    qRegisterMetaTypeStreamOperators<User>("User");
    qRegisterMetaType<QList<User> >("UserList");
    qRegisterMetaTypeStreamOperators<QList<User> > ("UserList");
    qInstallMessageHandler(0);

    if(argc >1 &&QString(argv[1])=="debug"){

    }else{
        FreeConsole();
    }
   // QQmlApplicationEngine engine;
 //   engine.load(QUrl(QStringLiteral("qrc:/video.qml")));

    MainWindow w;
      w.show();


    return a.exec();
}
