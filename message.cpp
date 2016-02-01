#include "message.h"

Message::Message():SerializableObject()
{

}

QMap<int, QPair<QString, QString> > Message::getLinks() const
{
    return links;
}

void Message::setLinks(const QMap<int, QPair<QString, QString> > &value)
{
    links = value;
}

int Message::getTimeAt() const
{
    return timeAt;
}

void Message::setTimeAt(int value)
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
