#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include "message.h"

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
    void addClient(int id, QTcpSocket);
    void removeClient(int id);
    void sendToAllClients(QString str);
public:
    QMap<int, QTcpSocket* >clients;
    explicit NetworkController(QObject *parent = 0);
    bool isHost(){
        return host;
    }
    bool isOnline(){return online;}
    void startServer(int port);
    void connectToHost(QString address, int port);

    void broadcastToClients(const Message &msg);
    void sendToHost(const Message &msg);

    int getClientId() const;
    void setClientId(int value);


signals:
    void newClientConnected(Message &msg);
    void messageComeIn(Message &msg);
private slots:
    //----server
    void newConnection();
    void clientDisconnected();
    //client----
    void connectedToHost();
    void clientRead();
    void clientSend(QString str);
    void hostRead();
};

#endif // NETWORKCONTROLLER_H
