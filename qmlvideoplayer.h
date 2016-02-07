#ifndef QMLVIDEOPLAYER_H
#define QMLVIDEOPLAYER_H

#include "videoplayer.h"

#include <QQuickView>
#include <QQuickWindow>
class QmlVideoPlayer : public VideoPlayer
{
Q_OBJECT
    QObject * qmlVideo;
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
private slots:
    void qmlStateChanged(int state);
     void qmlMediaStatusChanged(int status);
      void qmlDurationChanged(int duration);
      void qmlPositionChanged(int pos);
      void qmlAvailabilityChanged(int a);
    void testSlot(QString msg);
};


#endif // QMLVIDEOPLAYER_H
