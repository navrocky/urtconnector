#ifndef SERVER_LIST_UPDATER_H
#define SERVER_LIST_UPDATER_H

#include <QObject>
#include <QTime>

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

    void set_timeout(int val) {timeout_ = val;}
    void set_max_sim_queries(int val) {maximum_sim_queries_ = val;}
    void set_retries(int val) {retries_ = val;}

    void refresh_selected(const server_id_list& list);

    /* total count servers to refresh */
    int count() const {return count_;}

    /* refreshing progress */
    int progress() const {return progress_;}

    server_list_p server_list() const {return serv_list_;}

public slots:
    void refresh_cancel();

signals:
    void refresh_stopped();

private slots:
    void query_finished();
    void query_error(const QString&);
    void start_later();

private:
    struct server_rec
    {
        urt_get_server_info* info_query;
        urt_get_server_status* status_query;
    };

    typedef QMap<server_id, server_rec> server_recs_t;

    void clear();
    void query_portion();
    void prepare_info(server_info_p si);

    int current_id_;
    server_id_list id_list_;
    server_list_p serv_list_;
    server_recs_t queries_;
    int count_;
    int progress_;
    bool canceled_;
    geoip geoip_;
    server_info_list old_list_;
    urt_query_dispatcher* dispatcher_;
    int maximum_sim_queries_;
    server_id_list later_status_start_;
    int timeout_;
    int retries_;
    int started_;
    int finished_;
    int retries_counter_;
    int failed_counter_;
    QTime time_;
};

#endif // SERVER_LIST_UPDATER_H
