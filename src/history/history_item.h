#ifndef HISTORY_ITEM_H
#define HISTORY_ITEM_H

#include <QDateTime>
#include "common/server_id.h"

class history_item
{
protected:
    server_id id_;
    QString server_name_;
    QString password_;
    QString player_name_;
    QDateTime date_time_;
public:
    history_item(server_id& id, QString& server_name, QString& password, QString& player_name, QDateTime& date_time);

    server_id id() const;
    QString address() const;
    QString server_name() const;
    QString password() const;
    QString player_name() const;
    QDateTime date_time() const;
};

#endif // HISTORY_ITEM_H
