#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>

class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = 0);
    virtual void play()=0;
    virtual void pause()=0;
    virtual void seekTo(double sec)=0;
    virtual void load(QString path)=0;
signals:

public slots:
};

#endif // VIDEO_H
