#ifndef _JOB_UPDATE_SELECTED_H
#define	_JOB_UPDATE_SELECTED_H

#include <memory>
#include <QString>

#include "jobs/job.h"
#include "server_id.h"
#include "qstat_updater.h"
#include "pointers.h"

class qstat_updater;
class qstat_options;

class job_update_selected : public job_t
{
    Q_OBJECT
public:
    job_update_selected(const server_id_list& selection, 
                        server_list_p list,
                        qstat_options* opts);

    QString get_caption();
    void start();
    void cancel();
    int get_progress();

private slots:
    void stopped();

private:
    QString caption_;
    server_id_list selection_;
    std::auto_ptr<qstat_updater> updater_;
};

#endif	/* _JOB_UPDATE_SELECTED_H */
