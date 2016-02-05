#ifndef USER_H
#define USER_H
#include "QObject"

#include <QDataStream>
class User
{

public:
    User();
    int id =0;
    qint64 ping =0;
    int state =0;
signals:

public slots:
};
QDataStream &operator<<(QDataStream &ds, const User &obj);
QDataStream &operator>>(QDataStream &ds, User &obj) ;
Q_DECLARE_METATYPE(User)
#endif // USER_H
