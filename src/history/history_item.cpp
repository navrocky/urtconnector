
#include <QDateTime>

#include "common/server_id.h"
#include "history_item.h"

struct history_item::Pimpl
{
    server_id id;
    QString server_name;
    QString password;
    QString player_name;
    QDateTime timestamp;
};

history_item::history_item()
{}

history_item::history_item(const server_id& id, const QString& server_name,
                           const QString& password, const QString& player_name,
                           const QDateTime& timestamp)
{
    p_->id = id;
    p_->server_name = server_name;
    p_->password = password;
    p_->player_name = player_name;
    p_->timestamp = timestamp;
}


const server_id& history_item::id() const
{ return p_->id; }

const QString& history_item::server_name() const
{ return p_->server_name; }

const QString& history_item::password() const
{ return p_->password; }

const QString& history_item::player_name() const
{ return p_->player_name; }

const QDateTime& history_item::timestamp() const
{ return p_->timestamp; }

bool history_item::operator==(const history_item& other) const
{ return ( p_->id == other.p_->id ) && ( p_->timestamp == other.p_->timestamp ) && ( p_->player_name == other.p_->player_name ); }

bool history_item::operator<(const history_item& other) const
{
    if (p_->id == other.p_->id)
        return ( p_->timestamp < other.p_->timestamp );
    else
        return ( p_->id < other.p_->id );
}

bool history_item::is_empty() const
{ return p_->timestamp.isNull(); }

