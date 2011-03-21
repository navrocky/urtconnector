#include "friend_record.h"

friend_record::friend_record(const QString& nick_name,
                             const QString& regexp,
                             const QString& comment)
{
    d->nick_name_ = nick_name;
    d->regexp_ = regexp;
    d->comment_ = comment;
}

bool friend_record::operator<(const friend_record& src) const
{
    return d->nick_name_ < src.d->nick_name_;
}

bool friend_record::operator==(const friend_record& src) const
{
    return d->nick_name_ == src.d->nick_name_;
}
