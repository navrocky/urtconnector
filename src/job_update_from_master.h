#ifndef _JOB_UPDATE_FROM_MASTER_H
#define	_JOB_UPDATE_FROM_MASTER_H

#include <memory>
#include <QString>

#include "jobs/job.h"

class serv_list_custom;
class qstat_updater;
class qstat_options;

class job_update_from_master : public job_t
{
    Q_OBJECT
public:
    job_update_from_master(serv_list_custom* list, qstat_options* opts);
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

