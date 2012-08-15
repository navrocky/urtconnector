#ifndef _JOB_UPDATE_FROM_MASTER_H
#define	_JOB_UPDATE_FROM_MASTER_H

#include <memory>
#include <QString>

#include "pointers.h"
#include <jobs/job.h>
#include <server_query/urt_query.h>

class server_list;
class geoip;
class server_list_updater;

class job_update_from_master : public job_t
{
    Q_OBJECT
public:
    job_update_from_master(server_list_p list, const geoip& gi);
    ~job_update_from_master();
    
    void start();
    void cancel();
    int get_progress();

private slots:
    void stopped();
    void query_server_list_finished(const server_id_list&);
    void error(const QString&);

private:
    urt_query_dispatcher* dispatcher_;
    urt_get_server_list* get_list_query_;
    server_list_updater* updater_;
};

#endif	/* _JOB_UPDATE_FROM_MASTER_H */

