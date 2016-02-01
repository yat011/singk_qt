#ifndef MESSAGE_H
#define MESSAGE_H
#include <QMap>
#include "serializableobject.h"
 enum { HELLO=0, OK, PLAY} ;
class Message : public SerializableObject
{
private:
    int id = 0;

public:

    Message();
    int clientId = -1;
    int type = HELLO;
    int timeAt = -1;
    int currentId = -1;
    QMap<int, QPair<QString,QString>> links;



    int getType() const;
    void setType(int value);
    int getClientId() const;
    void setClientId(int value);
    int getCurrentId() const;
    void setCurrentId(int value);
    int getTimeAt() const;
    void setTimeAt(int value);
    QMap<int, QPair<QString, QString> > getLinks() const;
    void setLinks(const QMap<int, QPair<QString, QString> > &value);
};

#endif // MESSAGE_H
