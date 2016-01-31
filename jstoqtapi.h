#ifndef JSTOQTAPI_H
#define JSTOQTAPI_H

#include <QObject>
#include <QDebug>
#include <string>
class JsToQtApi : public QObject
{
    Q_OBJECT
public:
    explicit JsToQtApi(QObject *parent = 0);
   Q_INVOKABLE void debug(QString str){
        qDebug() << "debug";
        qDebug() << str;
    }


signals:
    void timeChanged(double time);
    void loaded(double duration);
    void ended();
    void youtubeApiReady();
public slots:
};

#endif // JSTOQTAPI_H
