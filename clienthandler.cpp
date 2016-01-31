#include "clienthandler.h"

#include <QDebug>
#include <QTcpSocket>
ClientHandler::ClientHandler(int clientId, QTcpSocket *socket, QObject *parent )
{
    this->clientId=clientId; this->socket = socket;
   // this->socket->setSocketDescriptor(socket->socketDescriptor());

}

void ClientHandler::run()
{
    // thread starts here
    qintptr tmp =socket->socketDescriptor();
    this->socket = new QTcpSocket;
    this->socket->setSocketDescriptor(tmp);
    qDebug() << " Thread started";


    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << clientId << " Client connected";
    QString msg = "welcome";
    socket->write(msg.toStdString().c_str(),msg.length());
    exec();
}

void ClientHandler::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() <<clientId << " Data in: " << Data;

    //->write(Data);
}

void ClientHandler::disconnected()
{
    qDebug() <<clientId<< " Disconnected";


    socket->deleteLater();
    exit(0);
}
