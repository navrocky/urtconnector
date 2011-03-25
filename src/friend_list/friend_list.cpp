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
    emit changed();
}

void friend_list::remove(const QString& nick_name)
{
    QList<QString> nn;
    nn.append(nick_name);
    remove(nn);
}

void friend_list::remove(const QList<QString> nick_names)
{
    QList<QString> removed_nicks;
    foreach (const QString& nn, nick_names)
    {
        if (friends_.remove(nn))
        {
            removed_nicks.append(nn);
        }
    }
    if (removed_nicks.size() > 0)
    {
        emit changed();
        emit removed(removed_nicks);
    }
}

void friend_list::change(const QString& old_nick_name, const friend_record& rec)
{
    if (old_nick_name != rec.nick_name())
        friends_.remove(old_nick_name);
    friends_[rec.nick_name()] = rec;
    emit changed(old_nick_name, rec.nick_name());
    emit changed();
}
