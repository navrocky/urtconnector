#ifndef _JOB_UPDATE_FROM_MASTER_H
#define	_JOB_UPDATE_FROM_MASTER_H

#include <memory>
#include <QString>

#include "pointers.h"
#include "jobs/job.h"

class server_list;
class qstat_updater;
class qstat_options;

class job_update_from_master : public job_t
{
    Q_OBJECT
public:
    job_update_from_master(server_list_p list, qstat_options* opts);
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

