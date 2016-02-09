#ifndef QMLVIDEOPLAYER_H
#define QMLVIDEOPLAYER_H

#include "videoplayer.h"

#include <QMediaPlayer>
#include <QQuickView>
#include <QQuickWindow>
class QmlVideoPlayer : public VideoPlayer
{
Q_OBJECT
    QObject * qmlVideo;
    QObject *player;
    QMediaPlayer * qp;
public:
    QmlVideoPlayer(QObject * qml, QQuickView *window, QObject * parent=0);
    void play();
    void pause();
    void stop();
    void seekTo(qint64 pos);
    void load(QString path);
    int position();
    int state();
     int mediaStatus();
    QString getErrorString();
    void setMedia(QUrl url);
    bool isVideoAvailable();
    qint64 duration();
    void setVolume(int v);
    int volume();
    QString source();
    void showFlashMessage(QString msg);
    void setInfoMsg(QString msg);
    void reset();
    bool random();
    void setRandom(bool r);
signals:
    void playPasueButtonClicked();
    void timeSliderReleased(int);
    void forwardButtonClicked();
    void orderButtonClicked();
private slots:
    void qmlStateChanged(QMediaPlayer::State state);
     void qmlMediaStatusChanged(QMediaPlayer::MediaStatus status);
      void qmlDurationChanged(qint64 duration);
      void qmlPositionChanged(qint64 pos);
      void qmlAvailabilityChanged(bool a);
    void testSlot();
    void qmlVolumeChanged(int v);
};


#endif // QMLVIDEOPLAYER_H
