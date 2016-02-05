#include "user.h"

User::User()
{

}

QDataStream &operator<<(QDataStream &ds, const User &obj)
{
    ds << obj.id << obj.ping <<obj.state;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, User &obj)
{
    ds >>obj.id >> obj.ping >>obj.state;
    return ds;
}
