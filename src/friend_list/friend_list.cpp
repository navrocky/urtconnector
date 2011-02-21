#include <qmap.h>

#include "friend_list.h"

friend_list::friend_list(QObject* parent)
: QObject(parent)
{
}

void friend_list::add(const friend_record& rec)
{
    friends_[rec.nick_name()] = rec;
    emit added(rec.nick_name());
}

void friend_list::remove(const QString& nick_name)
{
    if (friends_.remove(nick_name) > 0)
        emit removed(nick_name);
}

void friend_list::change(const QString& old_nick_name, const friend_record& rec)
{
    if (old_nick_name != rec.nick_name())
        friends_.remove(old_nick_name);
    friends_[rec.nick_name()] = rec;
    emit changed(old_nick_name, rec.nick_name());
}
