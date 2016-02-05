#include "message.h"

int Message::getSeq() const
{
    return seq;
}

void Message::setSeq(int value)
{
    seq = value;
}

QMap<int, QPair<QString, QString> > Message::getLinks() const
{
    QMap<int, QPair<QString, QString> > res;
    for(QString e : links.keys()){
        res[e.toInt()] =links[e].value<QPair<QString, QString> >() ;
    }
    return res;
}

qint64 Message::getTimeStamp() const
{
    return timeStamp;
}

void Message::setTimeStamp(const qint64 &value)
{
    timeStamp = value;
}

Message::Message():SerializableObject()
{
    
}

QList<User> Message::getUsers() const
{
    return users;
}

void Message::setUsers(const QList<User> &value)
{
    users = value;
}

int Message::getOptId() const
{
    return optId;
}

void Message::setOptId(int value)
{
    optId = value;
}

int Message::getCurrentState() const
{
    return currentState;
}

void Message::setCurrentState(int value)
{
    currentState = value;
}

QMap<QString, QVariant> Message::getVariantLinks() const
{
    return links;
}

void Message::setVariantLinks(const QMap<QString, QVariant> &value)
{
    links = value;
}

void Message::setLinks(const QMap<int, QPair<QString, QString> > &value)
{
    links = QMap<QString,QVariant>();
    for(int e : value.keys()){
        links[QString::number(e)] = QVariant::fromValue(value[e]);
    }
}

void Message::addLink(int id, QPair<QString, QString> pair)
{
    links[QString::number(id)] = QVariant::fromValue(pair);
}

QPair<QString, QString> Message::getLink(int id)
{
    return links[QString::number(id)].value<QPair<QString, QString> >();
}


qint64 Message::getTimeAt() const
{
    return timeAt;
}

void Message::setTimeAt(qint64 value)
{
    timeAt = value;
}

int Message::getCurrentId() const
{
    return currentId;
}

void Message::setCurrentId(int value)
{
    currentId = value;
}

int Message::getClientId() const
{
    return clientId;
}

void Message::setClientId(int value)
{
    clientId = value;
}

int Message::getType() const
{
    return type;
}

void Message::setType(int value)
{
    type = value;
}

