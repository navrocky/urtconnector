#ifndef HISTORY_H
#define HISTORY_H

#include <list>
#include "history_item.h"
#include "pointers.h"
#include "common/server_id.h"

class history
{
public:
    history();

    typedef std::list<history_item_p> HistoryList;

    void add(server_id id, QString server_name, QString player_name, QString password);

    void remove( history_item_p item) {
        list_.remove(item);
        save();
    }

    const HistoryList& list() const {
        return list_;
    }
    
    void change_max();

private:
    void add_history_item(history_item_p item);
    void add_from_file(QDateTime date_time, QString server_name, QString address, QString password, QString player_name);
    void save();
    void load();
    void shorten();

    HistoryList list_;
    unsigned int max_;
    qsettings_p history_file_;
};

#endif // HISTORY_H
