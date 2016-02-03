#include "videodestroyer.h"
#include <QFile>
#include <QDir>
#include <QDebug>
int VideoDestroyer::getBuffer() const
{
    return buffer;
}

void VideoDestroyer::setBuffer(int value)
{
    buffer = value;
}

void VideoDestroyer::pushVideo(QString name)
{
    if (buffer == -1){
        return;
    }
    qDebug() << "destroy: push " << name;
    list << name;
    if (list.size() > buffer){
        QString d = list[0];
        list.removeFirst();
        if (QFile::remove(QDir::currentPath()+"/videos/"+d+".mp4")){
            qDebug() <<"deleted " << name;
        }else{
            qDebug() <<"fail to delete " << name;
        }
    }
}

VideoDestroyer::VideoDestroyer(QObject *parent) : QObject(parent)
{

}

VideoDestroyer::~VideoDestroyer()
{
    for (QString str : list){
        if (QFile::remove(QDir::currentPath()+"/videos/"+str+".mp4")){
            qDebug() <<"deleted " << str;
        }else{
            qDebug() <<"fail to delete " << str;
        }
    }
}
