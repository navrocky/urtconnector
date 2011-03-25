#include "friend_record.h"

bool friend_record::operator<(const friend_record& src) const
{
    return d->nick_name_ < src.d->nick_name_;
}

bool friend_record::operator==(const friend_record& src) const
{
    return d->nick_name_ == src.d->nick_name_;
}
