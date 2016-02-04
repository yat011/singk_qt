#include "networkcontroller.h"
#include "clienthandler.h"
#include "message.h"

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
        //welcome msg
        Message msg;
        msg.setType(HELLO);
        msg.setClientId(socket->socketDescriptor());
        emit newClientConnected(msg);
        QByteArray arr;
        QDataStream out (&arr, QIODevice::WriteOnly);
        out << msg;

        socket->write(arr);



}

void NetworkController::hostRead(){
    QTcpSocket * socket = (QTcpSocket*) sender();
     QDataStream in(socket->readAll());
     Message msg ;
     in >> msg;
     if (in.atEnd()){
         qDebug() <<"no msg";
     }else{
         qDebug() << "still hv msg";
     }
     qDebug() << "host receive msg";
     emit messageComeIn(msg);
}

void NetworkController::clientDisconnected()
{
    QTcpSocket * socket = (QTcpSocket*) sender();
    int id = socket->property("id").value<int>();
    qDebug() << "client " << id << " disconnected";
    clients.remove(id);

}

void NetworkController::clientRead()
{

     QDataStream in(cSocket->readAll());
     Message msg ;
     in >> msg;
      if (in.atEnd()){
          qDebug() <<"no msg";
      }else{
          qDebug() << "still hv msg";
      }
     if (msg.getType() == HELLO){
        qDebug() << "client Id" << msg.getClientId() << " come";
        clientId = msg.getClientId();
        emit messageComeIn(msg);
     }else{
         qDebug() << "client" << clientId << " receive msg " << msg.getType();
         emit messageComeIn(msg);
     }

}

void NetworkController::clientSend(QString str)
{

}
void NetworkController::connectToHost(QString address, int port)
{
    if (online){
        qDebug() << "cannot connect again" ;
        return;
    }
    cSocket = new QTcpSocket(this);
    cSocket->connectToHost(address,port);
    connect(cSocket, SIGNAL(connected()),this,SLOT(connectedToHost()));
}

void NetworkController::broadcastToClients(const Message &msg)
{
   for (QTcpSocket* socket: clients.values()) {
        QByteArray arr;
        QDataStream out (&arr, QIODevice::WriteOnly);
        out << msg;
        socket->write(arr);
        if (socket->flush()){
           // qDebug() << msg.getType() << " written";
        }
   }

}

void NetworkController::sendToHost(const Message &msg)
{
       QByteArray arr;
        QDataStream out (&arr, QIODevice::WriteOnly);
        out << msg;
        cSocket->write(arr);

}

void NetworkController::connectedToHost() //client
{
    qDebug() << "connected to Host";
    online= true;
    host =false;
    emit onlineSig(host);
    connect(cSocket, SIGNAL(readyRead()),this,SLOT(clientRead()));
    connect(cSocket, SIGNAL(disconnected()),this,SLOT(onDisconnectedFromHost()));
}

void NetworkController::onDisconnectedFromHost()
{
    online= false;
    emit offline();

}
