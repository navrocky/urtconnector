#ifndef _SERVER_LIST_H
#define _SERVER_LIST_H

#include <QObject>

#include "server_info.h"
#include "server_id.h"

/*! Server list, used by server_list_widget to fetching server_info. */
class server_list : public QObject
{
    Q_OBJECT
public:
    server_list(QObject* parent = NULL);

    const server_info_list& list() const {return list_;}

    server_info_p get(const server_id& id);

    void add(server_info_p si);

    /*! Call this after any internal info change. */
    void state_changed();

    void remove_all();
    void remove_selected(const server_id_list& ids);
    void remove_old();

signals:
    void changed();

private:
    server_info_list list_;
};

#endif
