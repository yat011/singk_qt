#ifndef SERIALIZABLEOBJECT_H
#define SERIALIZABLEOBJECT_H

#include <QObject>
#include <QDataStream>
#include <QMetaProperty>
class SerializableObject : public QObject
{
    Q_OBJECT
public:
    explicit SerializableObject(QObject *parent = 0);

signals:

public slots:
};
QDataStream &operator<<(QDataStream &ds, const SerializableObject &obj);
QDataStream &operator>>(QDataStream &ds, SerializableObject &obj) ;
#endif // SERIALIZABLEOBJECT_H
