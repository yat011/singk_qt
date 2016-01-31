#include "networkcontroller.h"
#include "clienthandler.h"

void NetworkController::addClient(int id, QTcpSocket)
{

}

void NetworkController::removeClient(int id)
{

}

void NetworkController::sendToAllClients(QString str)
{

}

NetworkController::NetworkController(QObject *parent) : QObject(parent)
{

}

void NetworkController::startServer(int port)
{
    host =true;
    online= true;
    server = new QTcpServer();
    if(!server->listen(QHostAddress::Any, port))
        {
            qDebug() << "Could not start server";
        }
        else
        {
            qDebug() << "Listening to port " << port << "...";
           connect(server,SIGNAL(newConnection()),this, SLOT(newConnection()));

    }
}

void NetworkController::connectToHost(QString address, int port)
{
    cSocket = new QTcpSocket(this);
    cSocket->connectToHost(address,port);
    connect(cSocket, SIGNAL(connected()),this,SLOT(connectedToHost()));
}

void NetworkController::connectedToHost()
{
    qDebug() << "connected to Host";
    connect(cSocket, SIGNAL(readyRead()),this,SLOT(clientRead()));
}


void NetworkController::newConnection()
{

        qDebug() << "new connection";
        // Every new connection will be run in a newly created thread
        ClientHandler*thread = new ClientHandler(clientId++, server->nextPendingConnection());

        // connect signal/slot
        // once a thread is not needed, it will be beleted later
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
}

void NetworkController::clientRead()
{
    QString msg = QString(cSocket->readAll());
    qDebug() << "client read:" << msg;
}

void NetworkController::clientSend(QString str)
{

}
