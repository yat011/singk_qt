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


    void saveBlockSize(quint32 blockSize, QTcpSocket *socket);
    quint32 getBlockSize(QTcpSocket *socket);
public:
    QMap<int, QTcpSocket* >clients;
    QMap<int,quint32>nextBlockSize;
    explicit NetworkController(QObject *parent = 0);
    bool isHost(){
        return host;
    }
    bool isOnline(){return online;}
    void startServer(int port);
    void connectToHost(QString address, int port);

    void broadcastToClients( Message &msg);
    void sendToHost( Message &msg);

    int getClientId() const;
    void setClientId(int value);
    void disconnectThis();


signals:
    void newClientConnected(Message &msg);
    void onlineSig(bool host);
    void offline();
    void serverStarted();
    void messageComeIn(Message &msg);
    void clientInitComplete();
    void consoleMessage(QString msg);
    void networkError();
    void clientDisconnectedSig(int id);
private slots:
    //----server
    void newConnection();
    void clientDisconnected();
    //client----

    void connectedToHost();
    void onDisconnectedFromHost();
    void clientRead();
    void hostRead();
    void onError(QAbstractSocket::SocketError err);
    void writeMessage(Message& msg, QTcpSocket * socket);
    void readMessage(QList<Message*>& out, QTcpSocket *socket);

};

#endif // NETWORKCONTROLLER_H
