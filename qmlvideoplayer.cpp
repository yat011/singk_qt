#include "qmlvideoplayer.h"
#include <QVariant>
#include <QDebug>
#include <QQuickWindow>
#include <QQuickView>
#include <QQuickItem>
#include <QMediaPlayer>
QmlVideoPlayer::QmlVideoPlayer(QObject *qml, QQuickView* window, QObject *parent):VideoPlayer(parent),qmlVideo(qml)
{
    QQuickItem * item = qobject_cast<QQuickItem*> (qml);


        player = qml->findChild<QObject*>("player");
       qp = qvariant_cast<QMediaPlayer *>(player->property("mediaObject"));
       /*
       QObject::connect(item,SIGNAL(stateChanged(int)),this,SLOT(qmlStateChanged(int)));
        QObject::connect(item,SIGNAL(mediaStatusChanged(int)),this,SLOT(qmlMediaStatusChanged(int)));
        QObject::connect(item,SIGNAL(videoDurationChanged(int)),this,SLOT(qmlDurationChanged(int)));
       QObject::connect(item,SIGNAL(videoPositionChanged(int)),this,SLOT(qmlPositionChanged(int)));
       QObject::connect(item,SIGNAL(videoAvailabilityChanged(bool)),this,SLOT(qmlAvailabilityChanged(bool)));
      // QQuickItem* playPauseBtn = qobject_cast<QQuickItem*>(qml->findChild<QObject*>("playPauseBtn"));
        QObject::connect(item,SIGNAL(playPasueButtonClicked()),this,SIGNAL(playPasueButtonClicked()));
           QObject::connect(item,SIGNAL(timeSliderReleased(int)),this,SIGNAL(timeSliderReleased(int)));
           */
       QObject::connect(item,SIGNAL(playPasueButtonClicked()),this,SIGNAL(playPasueButtonClicked()));
          QObject::connect(item,SIGNAL(timeSliderReleased(int)),this,SIGNAL(timeSliderReleased(int)));
          connect(qp,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(qmlStateChanged(QMediaPlayer::State)));
          connect(qp,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(qmlMediaStatusChanged(QMediaPlayer::MediaStatus)));
          QObject::connect(qp,SIGNAL(durationChanged(qint64)),this,SLOT(qmlDurationChanged(qint64)));
         QObject::connect(qp,SIGNAL(positionChanged(qint64)),this,SLOT(qmlPositionChanged(qint64)));
         QObject::connect(qp,SIGNAL(availabilityChanged(bool)),this,SLOT(qmlAvailabilityChanged(bool)));

}



void QmlVideoPlayer::play()
{
    qDebug() << "play plz";
  //  QMediaPlayer *qp = qvariant_cast<QMediaPlayer *>(player->property("mediaObject"));
    qp->play();
    // QMetaObject::invokeMethod(player,"play");
}

void QmlVideoPlayer::pause()
{
   // QMetaObject::invokeMethod(player,"pause");
    qp->pause();
}

void QmlVideoPlayer::stop()
{
    //QMetaObject::invokeMethod(player,"stop");
    qp->stop();
}

void QmlVideoPlayer::seekTo(qint64 pos)
{
    QVariant p = pos;
    //QMetaObject::invokeMethod(player,"seek",Q_ARG(int,pos));
    qp->setPosition(pos);
}

void QmlVideoPlayer::load(QString path)
{
   // player->setProperty("source", path);

}

int QmlVideoPlayer::position()
{
    return qp->position();

}

int QmlVideoPlayer::state()
{
return qp->state();
}

int QmlVideoPlayer::mediaStatus()
{
return qp->mediaStatus();
}

QString QmlVideoPlayer::getErrorString()
{
    return qp->errorString();
}

void QmlVideoPlayer::setMedia(QUrl url)
{
    qDebug()<<"set url"<<url;
   // player->setProperty("source", url.toString());
    qp->setMedia(url);


}

bool QmlVideoPlayer::isVideoAvailable()
{
    return qp->isVideoAvailable();
}



qint64 QmlVideoPlayer::duration()
{
    return qp->duration();
}

void QmlVideoPlayer::setVolume(int v)
{
    //double t = v/100.0;
   // player->setProperty("volume", t);
    qp->setVolume(v);
}

int QmlVideoPlayer::volume()
{
    //double v = player->property("volume").toDouble();
   // return (int)(v*100);
    return qp->volume();
}

QString QmlVideoPlayer::source()
{
    return qp->media().canonicalUrl().toString();
}

void QmlVideoPlayer::showFlashMessage(QString msg)
{
    QVariant m= msg;
    QVariant d= 3000;
    QMetaObject::invokeMethod(qmlVideo,"showFlashMessage",Q_ARG(QVariant,m),Q_ARG(QVariant,d));
}

void QmlVideoPlayer::setInfoMsg(QString msg)
{
    QVariant m= msg;
    QMetaObject::invokeMethod(qmlVideo,"setInfoMessage",Q_ARG(QVariant,m));
}

void QmlVideoPlayer::reset()
{
   // setMedia(QUrl());
    //QMediaPlayer *qp = qvariant_cast<QMediaPlayer *>(player->property("mediaObject"));
   // qp->setMedia(QMediaContent());
    //QMediaPlayer *localQMediaPlayer = new QMediaPlayer();
   // QVariant localFromValue = QVariant.fromValue(*localQMediaPlayer);
    //player->setProperty("mediaObject",localFromValue);
}

void QmlVideoPlayer::qmlStateChanged(QMediaPlayer::State state)
{
    emit stateChanged(state);
}

void QmlVideoPlayer::qmlMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    emit mediaStatusChanged(status);
}

void QmlVideoPlayer::qmlDurationChanged(qint64 duration)
{
    emit durationChanged(duration);
}

void QmlVideoPlayer::qmlPositionChanged(qint64 pos)
{
    emit positionChanged(pos);
}

void QmlVideoPlayer::qmlAvailabilityChanged(bool a)
{
    emit videoAvailableChanged(a);
}

void QmlVideoPlayer::testSlot()
{
    qDebug()<<"test slot:";
}

void QmlVideoPlayer::qmlVolumeChanged(int v)
{
    emit volumeChanged(v);
}


