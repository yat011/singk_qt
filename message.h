#ifndef MESSAGE_H
#define MESSAGE_H
#include <QMap>
#include "serializableobject.h"
#include <QPair>
#include "definition.h"
#include  "user.h"
#include <QList>
typedef QList<User> UserList;
class Message : public SerializableObject
{
    Q_OBJECT

    Q_PROPERTY(int seq READ getSeq WRITE setSeq)
    Q_PROPERTY(int clientId READ getClientId WRITE setClientId)
    Q_PROPERTY(qint64 timeAt READ getTimeAt WRITE setTimeAt)
    Q_PROPERTY(int currentId READ getCurrentId WRITE setCurrentId)
    Q_PROPERTY(int type READ getType WRITE setType)
    Q_PROPERTY(int currentState READ getCurrentState WRITE setCurrentState)
    Q_PROPERTY(QVariantMap links READ getVariantLinks WRITE setVariantLinks)
    Q_PROPERTY(int optId READ getOptId WRITE setOptId)
    Q_PROPERTY(qint64 timeStamp READ getTimeStamp WRITE setTimeStamp)
    Q_PROPERTY(qint64 timeStamp READ getTimeStamp WRITE setTimeStamp)
    Q_PROPERTY(UserList users READ getUsers WRITE setUsers)
private:
    int seq = 0;
    qint64 timeStamp =0;
public:

    Message();
    int clientId = -1;
    int type = NONE;
    int currentState = PAUSE;
    qint64 timeAt = -1;
    int currentId = -1;
    int optId = -1;
    QMap<QString, QVariant > links;
    QList<User> users;

    void copyTo(Message & msg){
        msg.setSeq(seq);
        msg.setClientId(clientId);
        msg.setType(type);
        msg.setCurrentId(currentId);
        msg.setCurrentState(currentState);
        msg.setVariantLinks(links);
        msg.setOptId(optId);

    }

    int getType() const;
    void setType(int value);
    int getClientId() const;
    void setClientId(int value);
    int getCurrentId() const;
    void setCurrentId(int value);
    qint64 getTimeAt() const;
    void setTimeAt(qint64 value);

    int getSeq() const;
    void setSeq(int value);

    QMap<QString, QVariant> getVariantLinks() const;
    QMap<int, QPair<QString,QString> > getLinks() const;
    void setVariantLinks(const QMap<QString, QVariant> &value);
    void setLinks(const QMap<int, QPair<QString,QString> > &value);
    void addLink(int id,QPair<QString,QString> pair );
    QPair<QString,QString> getLink(int id );
    int getCurrentState() const;
    void setCurrentState(int value);

    int getOptId() const;
    void setOptId(int value);
    qint64 getTimeStamp() const;
    void setTimeStamp(const qint64 &value);
    QList<User> getUsers() const;
    void setUsers(const QList<User> &value);
};

#endif // MESSAGE_H
