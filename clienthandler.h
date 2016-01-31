#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
class ClientHandler : public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(int clientId, QTcpSocket *socket, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    int clientId =-1;

};

#endif // CLIENTHANDLER_H
