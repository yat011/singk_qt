#include "qmlvideoplayer.h"
#include <QVariant>
#include <QDebug>
#include <QQuickWindow>
#include <QQuickView>
#include <QQuickItem>
QmlVideoPlayer::QmlVideoPlayer(QObject *qml, QQuickView* window, QObject *parent):VideoPlayer(parent),qmlVideo(qml)
{
    QQuickItem * item = qobject_cast<QQuickItem*> (qml);
    QObject::connect(item,SIGNAL(stateChanged(int)),this,SLOT(qmlStateChanged(int)));
     QObject::connect(item,SIGNAL(mediaStatusChanged(int)),this,SLOT(qmlMediaStatusChanged(int)));
     QObject::connect(item,SIGNAL(videoDurationChanged(int)),this,SLOT(qmlDurationChanged(int)));
    QObject::connect(item,SIGNAL(videoPositionChanged(int)),this,SLOT(qmlPositionChanged(int)));
    QObject::connect(item,SIGNAL(videoAvailabilityChanged(int)),this,SLOT(qmlAvailabilityChanged(int)));
}

void QmlVideoPlayer::play()
{
    qDebug() << "play plz";
    QMetaObject::invokeMethod(qmlVideo,"play");
}

void QmlVideoPlayer::pause()
{
    QMetaObject::invokeMethod(qmlVideo,"pause");
}

void QmlVideoPlayer::stop()
{
    QMetaObject::invokeMethod(qmlVideo,"stop");
}

void QmlVideoPlayer::seekTo(qint64 pos)
{
    QVariant p = pos;
    QMetaObject::invokeMethod(qmlVideo,"seek",Q_ARG(QVariant,p));
}

void QmlVideoPlayer::load(QString path)
{
    qmlVideo->setProperty("source", path);
}

int QmlVideoPlayer::position()
{
    return qmlVideo->property("position").toInt();
}

int QmlVideoPlayer::state()
{
return qmlVideo->property("playbackState").toInt();
}

int QmlVideoPlayer::mediaStatus()
{
return qmlVideo->property("status").toInt();
}

QString QmlVideoPlayer::getErrorString()
{
    return qmlVideo->property("errorString").toInt();
}

void QmlVideoPlayer::setMedia(QUrl url)
{
    qmlVideo->setProperty("source", url.toString());
}

bool QmlVideoPlayer::isVideoAvailable()
{
   int s= qmlVideo->property("availability").toInt();
   return s==0;
}

qint64 QmlVideoPlayer::duration()
{
    return qmlVideo->property("duration").toInt();
}

void QmlVideoPlayer::setVolume(int v)
{
    double t = v/100.0;
    qmlVideo->setProperty("volume", t);
}

int QmlVideoPlayer::volume()
{
    double v = qmlVideo->property("volume").toDouble();
    return (int)(v*100);
}

void QmlVideoPlayer::qmlStateChanged(int state)
{
    emit stateChanged(state);
}

void QmlVideoPlayer::qmlMediaStatusChanged(int status)
{
    emit mediaStatusChanged(status);
}

void QmlVideoPlayer::qmlDurationChanged(int duration)
{
    emit durationChanged(duration);
}

void QmlVideoPlayer::qmlPositionChanged(int pos)
{
    emit positionChanged(pos);
}

void QmlVideoPlayer::qmlAvailabilityChanged(int a)
{
    qDebug() << "ava " << a;
    emit videoAvailableChanged(a==0);

}

void QmlVideoPlayer::testSlot(QString msg)
{
    qDebug()<<"test slot:"<<msg;
}


