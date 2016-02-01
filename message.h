#ifndef MESSAGE_H
#define MESSAGE_H
#include <QMap>
#include "serializableobject.h"
 enum { HELLO=0, OK, PLAY} ;
class Message : public SerializableObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ getId WRITE setId)
    Q_PROPERTY(int clientId READ getClientId WRITE setClientId)
    Q_PROPERTY(int timeAt READ getTimeAt WRITE setTimeAt)
    Q_PROPERTY(int currentId READ getCurrentId WRITE setCurrentId)
    Q_PROPERTY(int type READ getType WRITE setType)

private:
    int id = 0;

public:

    Message();
    int clientId = -1;
    int type = HELLO;
    int timeAt = -1;
    int currentId = -1;
    QMap<int, QPair<QString,QString> > links;



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
