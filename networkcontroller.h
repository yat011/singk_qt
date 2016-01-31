#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>

class NetworkController : public QObject
{
    Q_OBJECT
private:
    bool host = false;
    bool online = false;
public:
    explicit NetworkController(QObject *parent = 0);
    bool isHost(){
        return host;
    }
    bool isOnline(){return online;}

signals:

public slots:
};

#endif // NETWORKCONTROLLER_H
