#ifndef URT_HISTORY_H
#define URT_HISTORY_H

#include <boost/circular_buffer.hpp>

#include <QObject>

#include <common/server_id.h>

#include "history_item.h"

class history : public QObject
{
    Q_OBJECT
public:
    history(QObject* parent = 0);

    typedef boost::circular_buffer<history_item> history_list_t;

    void add( const history_item& item );
    
    void add( const server_id& id,
              const QString& server_name,
              const QString& player_name,
              const QString& password );


    void remove(const history_item& item);
    
    void clear();

    const history_list_t& list() const {return list_;}

    /*! Call this when options changed through dialog */
    void options_refresh();

Q_SIGNALS:
    /*! anuthing changed in history */
    void changed();

private Q_SLOTS:
    void save() const;
    
private:
    void load();
    void drop_old();

private:
    history_list_t list_;
    int max_days_;
};

#endif // HISTORY_H
