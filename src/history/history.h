#ifndef HISTORY_H
#define HISTORY_H

#include <list>
#include <QObject>
#include <common/server_id.h>
#include "history_item.h"
#include "../pointers.h"

class history : public QObject
{
    Q_OBJECT
public:
    history(QObject* parent = 0);

    typedef std::list<history_item> history_list_t;

    void add(const history_item& item);
    
    void add(const server_id& id,
             const QString& server_name,
             const QString& player_name,
             const QString& password);


    void remove(const history_item& item);
    void clear();

    const history_list_t& list() const {return list_;}

    /*! Call this when options changed through dialog */
    void options_refresh();

signals:
    /*! list changed */
    void changed();

private:
    void add_from_file(const QDateTime& date_time, const QString& server_name,
                       const QString& address, const QString& password,
                       const QString& player_name);
    void save();
    void load();
    void shorten();

    history_list_t list_;
    int max_records_;
    int max_days_;
    qsettings_p history_file_;
};

#endif // HISTORY_H
