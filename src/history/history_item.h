#ifndef HISTORY_ITEM_H
#define HISTORY_ITEM_H

#include <QDateTime>
#include <common/implicit_sharing.h>
#include <common/server_id.h>

class history_item
{
public:
    history_item();

    history_item(const server_id& id,
                 const QString& server_name,
                 const QString& password,
                 const QString& player_name,
                 const QDateTime& date_time);

    const server_id& id() const {return d->id_;}
    const QString& server_name() const {return d->server_name_;}
    const QString& password() const {return d->password_;}
    const QString& player_name() const {return d->player_name_;}
    const QDateTime& date_time() const {return d->date_time_;}

    bool operator==(const history_item& other) const;

protected:
    struct impl
    {
        server_id id_;
        QString server_name_;
        QString password_;
        QString player_name_;
        QDateTime date_time_;
    };
    implicit_sharing<impl> d;
};

#endif // HISTORY_ITEM_H
