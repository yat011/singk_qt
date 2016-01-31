#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
class NetworkController : public QObject
{
    Q_OBJECT
private:
    bool host = false;
    bool online = false;
    QTcpServer * server ;
    QTcpSocket * cSocket;
    int clientId = 0;
    QMap<int, QTcpSocket* >clients;
    QMutex mutex;
    void addClient(int id, QTcpSocket);
    void removeClient(int id);
    void sendToAllClients(QString str);
public:
    explicit NetworkController(QObject *parent = 0);
    bool isHost(){
        return host;
    }
    bool isOnline(){return online;}
    void startServer(int port);
    void connectToHost(QString address, int port);

signals:

private slots:
    void connectedToHost();
    void newConnection();
    void clientRead();
    void clientSend(QString str);
};

#endif // NETWORKCONTROLLER_H
