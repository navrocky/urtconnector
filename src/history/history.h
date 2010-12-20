#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include "history_item.h"
#include "pointers.h"
#include "common/server_id.h"

class history
{
public:
    history();

    void add(server_id id, QString server_name, QString player_name, QString password);

    void remove(int i)
    {
        list_.removeAt(i);
    }

    history_item_p at(int i)
    {
        return list_.at(i);
    }

    int length()
    {
        return list_.length();
    }

    void change_max();

private:
    void add_history_item(history_item_p item);
    void add_from_file(QDateTime date_time, QString server_name, QString address, QString password, QString player_name);
    void save();
    void load();
    void shorten();

    QList<history_item_p> list_;
    unsigned int max_;
    qsettings_p history_file_;
};

#endif // HISTORY_H
