#ifndef _JOB_UPDATE_FROM_MASTER_H
#define	_JOB_UPDATE_FROM_MASTER_H

#include <memory>
#include <QString>

#include "pointers.h"
#include "jobs/job.h"

class server_list;
class qstat_updater;
class qstat_options;
class geoip;

class job_update_from_master : public job_t
{
    Q_OBJECT
public:
    job_update_from_master(server_list_p list, const geoip& gi, qstat_options* opts);
    ~job_update_from_master();
    
    QString get_caption();
    void start();
    void cancel();
    int get_progress();
private slots:
    void stopped();

private:
    QString caption_;
    std::auto_ptr<qstat_updater> updater_;
};

#endif	/* _JOB_UPDATE_FROM_MASTER_H */

