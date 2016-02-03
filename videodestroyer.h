#ifndef VIDEODESTROYER_H
#define VIDEODESTROYER_H

#include <QObject>

class VideoDestroyer : public QObject
{
    Q_OBJECT
private:
    int buffer = 2;
    QStringList list;
public:
    explicit VideoDestroyer(QObject *parent = 0);
    ~VideoDestroyer();
    int getBuffer() const;
    void setBuffer(int value);
    void pushVideo(QString name);

signals:

public slots:
};

#endif // VIDEODESTROYER_H
