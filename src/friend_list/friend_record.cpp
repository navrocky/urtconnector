#include "friend_record.h"

friend_record::friend_record()
{
    d->use_regexp_ = false;
}

friend_record::friend_record(const QString& nick_name,
                             const QString& expr,
                             bool use_regexp,
                             const QString& comment)
{
    d->nick_name_ = nick_name;
    d->expression_ = expr;
    d->use_regexp_ = use_regexp;
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
