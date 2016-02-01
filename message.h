#ifndef MESSAGE_H
#define MESSAGE_H
#include <QMap>
#include "serializableobject.h"
#include <QPair>
#include "definition.h"

class Message : public SerializableObject
{
    Q_OBJECT

    Q_PROPERTY(int seq READ getSeq WRITE setSeq)
    Q_PROPERTY(int clientId READ getClientId WRITE setClientId)
    Q_PROPERTY(double timeAt READ getTimeAt WRITE setTimeAt)
    Q_PROPERTY(int currentId READ getCurrentId WRITE setCurrentId)
    Q_PROPERTY(int type READ getType WRITE setType)
    Q_PROPERTY(int currentState READ getCurrentState WRITE setCurrentState)
    Q_PROPERTY(QMap<QString, QVariant > links READ getVariantLinks WRITE setVariantLinks)

private:
    int seq = 0;

public:

    Message();
    int clientId = -1;
    int type = HELLO;
    int currentState = PAUSE;
    double timeAt = -1;
    int currentId = -1;
    QMap<QString, QVariant > links;



    int getType() const;
    void setType(int value);
    int getClientId() const;
    void setClientId(int value);
    int getCurrentId() const;
    void setCurrentId(int value);
    double getTimeAt() const;
    void setTimeAt(double value);

    int getSeq() const;
    void setSeq(int value);

    QMap<QString, QVariant> getVariantLinks() const;
    QMap<int, QPair<QString,QString>> getLinks() const;
    void setVariantLinks(const QMap<QString, QVariant> &value);
    void setLinks(const QMap<int, QPair<QString,QString> > &value);
    int getCurrentState() const;
    void setCurrentState(int value);
};

#endif // MESSAGE_H
