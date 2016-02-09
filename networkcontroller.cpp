#include "networkcontroller.h"
#include "clienthandler.h"
#include "message.h"
#include <QMessageBox>

int NetworkController::getClientId() const
{
    return clientId;
}

void NetworkController::setClientId(int value)
{
    clientId = value;
}

void NetworkController::disconnectThis()
{
    if(host){
        server->close();
        for (int key : clients.keys()){
            clients[key]->disconnect();
        }
        clients.clear();
        online = false;
        emit offline();
    }else {
        cSocket->disconnect();
    }
}




NetworkController::NetworkController(QObject *parent) : QObject(parent)
{

}

void NetworkController::startServer(int port)
{
    if (online){
        qDebug() << "cannot start server" ;
        return;
    }


    server = new QTcpServer();
    if(!server->listen(QHostAddress::Any, port))
     {
            qDebug() << "Could not start server";
            QMessageBox::critical(0,"Network err", server->errorString());
            emit networkError();
            server->deleteLater();
        }
        else
        {
            host =true;
            online= true;
            qDebug() << "Listening to port " << port << "...";
           connect(server,SIGNAL(newConnection()),this, SLOT(newConnection()));

           host = true;
           online =true;
           emit onlineSig(host);
           emit serverStarted();
    }
}




void NetworkController::newConnection()
{

        qDebug() << "new connection";
        QTcpSocket *socket = server->nextPendingConnection();
        socket->setProperty("id",socket->socketDescriptor());
        connect(socket,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
        connect(socket,SIGNAL(readyRead()),this,SLOT(hostRead()));


        clients.insert(socket->socketDescriptor(),socket);
        nextBlockSize[socket->socketDescriptor()] =0;

        //welcome msg
        Message msg;
        msg.setType(HELLO);
        msg.setClientId(socket->socketDescriptor());

        emit newClientConnected(msg);

        writeMessage(msg,socket);



}

void NetworkController::hostRead(){
    QTcpSocket* socket = (QTcpSocket*) sender();
    QList <Message*> out;
    readMessage(out,socket);
    for (Message * msg : out){
        emit messageComeIn(*msg);
         msg->deleteLater();
    }

}

void NetworkController::onError(QAbstractSocket::SocketError err)
{
    QTcpSocket *socket = (QTcpSocket*) sender();
    qDebug()<<"net err" << err;
    QMessageBox::critical(0,"Network err", socket->errorString());
    emit networkError();

}

void NetworkController::writeMessage(Message &msg, QTcpSocket *socket)
{
   // qDebug() << "start write message";
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out<<(quint32) 0;
    out<< msg;
    out.device()->seek(0);
    out <<(quint32)(block.size()-sizeof(quint32));

    socket->write(block);
    socket->flush();
   // qDebug() << "written a block";

}
void NetworkController::saveBlockSize(quint32 blockSize, QTcpSocket *socket){
    if (isHost()){
       nextBlockSize[socket->property("id").value<int>()]=blockSize;
    }else{
        nextBlockSize[0]=blockSize;
    }
}
quint32 NetworkController::getBlockSize( QTcpSocket *socket){
    quint32 nb = 0;
    if (isHost()){
       nb = nextBlockSize[socket->property("id").value<int>()];
    }else{
        nb = nextBlockSize[0];
    }
    return nb;

}
void NetworkController::readMessage(QList<Message*>& out, QTcpSocket *socket)
{
    QDataStream in(socket);

    quint32 nb = getBlockSize(socket);
   // qDebug() << "start read block (remain:" << nb;
    while(true){
        if (nb==0){
            if (socket->bytesAvailable() <sizeof(quint32)){
               // qDebug()<<"next block size not ready to read";
                break;
            }
            in >> nb;
        }
        if (socket->bytesAvailable() < nb){
            qDebug() << "wait for the whole Message";
           break;
        }
        Message *msg = new Message;
        in >> *msg;
       // qDebug() << "a Message read";
        out.append(msg);
        nb = 0;
    }
    saveBlockSize(nb,socket);
    return;
}

void NetworkController::clientDisconnected()
{
    QTcpSocket * socket = (QTcpSocket*) sender();
    int id = socket->property("id").value<int>();
    qDebug() << "client " << id << " disconnected";
    clients.remove(id);

    emit clientDisconnectedSig(id);
}

void NetworkController::clientRead()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    QList <Message*> out;
    readMessage(out,socket);
    for (Message * msg : out){
         if (msg->getType() == HELLO){
            qDebug() << "client Id" << msg->getClientId() << " come";
            clientId = msg->getClientId();
            qDebug() << "User" << msg->getUsers().size();
            emit messageComeIn(*msg);
            emit clientInitComplete();
         }else{
             emit messageComeIn(*msg);
         }
         msg->deleteLater();
    }

}


void NetworkController::connectToHost(QString address, int port)
{
    if (online){
        qDebug() << "cannot connect again" ;
        return;
    }
    cSocket = new QTcpSocket(this);
    connect(cSocket, SIGNAL(connected()),this,SLOT(connectedToHost()));
    connect(cSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));

    cSocket->connectToHost(address,port);

}

void NetworkController::broadcastToClients( Message &msg)
{
   for (QTcpSocket* socket: clients.values()) {
        writeMessage(msg,socket);
   }

}

void NetworkController::sendToHost(Message &msg)
{

        msg.setClientId(clientId);
        writeMessage(msg,cSocket);

}

void NetworkController::connectedToHost() //client
{
    qDebug() << "connected to Host";
    online= true;
    host =false;
    emit onlineSig(host);
    nextBlockSize[0]=0;
    connect(cSocket, SIGNAL(readyRead()),this,SLOT(clientRead()));
    connect(cSocket, SIGNAL(disconnected()),this,SLOT(onDisconnectedFromHost()));
}

void NetworkController::onDisconnectedFromHost()
{

    online= false;
    emit offline();

}
