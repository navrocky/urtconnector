#ifndef SERVER_LIST_UPDATER_H
#define SERVER_LIST_UPDATER_H

#include <QObject>

#include <common/server_info.h>
#include <geoip/geoip.h>
#include "../pointers.h"
#include "urt_query.h"

class urt_query_dispatcher;
class urt_get_server_info;
class urt_get_server_status;

class server_list_updater : public QObject
{
    Q_OBJECT
public:
    server_list_updater(server_list_p list, const geoip& gi, QObject* parent = 0);

    void refresh_selected(const server_id_list& list);

    /* total count servers to refresh */
//    int get_count() const {return count_;}

    /* refreshing progress */
//    int get_progress() const {return progress_;}

    void clear();

    /*! Clear offline items after full refresh */
    bool clear_offline() const {return clear_offline_;}
    void set_clear_offline(bool);

public slots:
    void refresh_cancel();

signals:
    void refresh_stopped();

private slots:
    void query_finished();

private:
    struct server_rec
    {
//        server_id id;
        urt_get_server_info* info_query;
        urt_get_server_status* status_query;
    };

    typedef QMap<server_id, server_rec> server_recs_t;

    void query_portion();
    void prepare_info(server_info_p si);

    int current_id_;
    server_id_list id_list_;
    int sim_queries_count_;
    server_list_p serv_list_;
    server_recs_t queries_;
//    int count_;
//    int progress_;
//    bool canceled_;
    geoip geoip_;

//    QString qstat_output_;
    bool clear_offline_;
    server_info_list old_list_;
    urt_query_dispatcher* dispatcher_;
    int maximum_sim_queries_;
};

#endif // SERVER_LIST_UPDATER_H
