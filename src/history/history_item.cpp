#include "history_item.h"

history_item::history_item(server_id& id, QString& server_name, QString& password, QString& player_name, QDateTime& date_time)
    : id_(id), server_name_(server_name), password_(password), player_name_(player_name), date_time_(date_time)
{
}

server_id history_item::id() const
{
    return id_;
}


QString history_item::address() const
{
    return id_.address();
}

QString history_item::server_name() const
{
    return server_name_;
}

QString history_item::password() const
{
    return password_;
}

QString history_item::player_name() const
{
    return player_name_;
}

QDateTime history_item::date_time() const
{
    return date_time_;
}
