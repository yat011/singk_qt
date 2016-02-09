#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QUrl>

class VideoPlayer : public QObject
{
    Q_OBJECT

public:
    explicit VideoPlayer(QObject *parent = 0);
    virtual void play()=0;
    virtual void pause()=0;
     virtual void stop()=0;
    virtual void seekTo(qint64 pos)=0;
    virtual void load(QString path)=0;
     virtual void setMedia(QUrl url)=0;
    virtual int position()=0;
    virtual int state()=0;
    virtual int mediaStatus()=0;
    virtual QString getErrorString()=0;
    virtual bool isVideoAvailable()=0;
    virtual qint64 duration()=0;
    virtual void setVolume(int v)=0;
    virtual int volume()=0;
    virtual QString source()=0;
    virtual void reset()=0;
signals:
    void positionChanged(qint64);
    void mediaStatusChanged(int);
    void stateChanged(int);
    void durationChanged(qint64);
    void videoAvailableChanged(bool);
    void volumeChanged(int);
public slots:
};

#endif // VIDEO_H
