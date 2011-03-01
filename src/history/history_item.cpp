#include "history_item.h"

history_item::history_item()
{}

history_item::history_item(const server_id& id, const QString& server_name,
                           const QString& password, const QString& player_name,
                           const QDateTime& date_time)
{
    d->id_ = id;
    d->server_name_ = server_name;
    d->password_ = password;
    d->player_name_ = player_name;
    d->date_time_ = date_time;
}

bool history_item::operator==(const history_item& other) const
{
    //player name ?
    return ( d->id_ == other.d->id_ ) && ( d->date_time_ == other.d->date_time_ );
}

