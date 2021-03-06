#ifndef _JOB_UPDATE_SELECTED_H
#define	_JOB_UPDATE_SELECTED_H

#include <QString>

#include <jobs/job.h>
#include <geoip/geoip.h>
#include <common/server_id.h>
#include "pointers.h"


class qstat_updater;
class qstat_options;
class server_list_updater;

class job_update_selected : public job_t
{
    Q_OBJECT
public:
    job_update_selected(const server_id_list& selection, 
                        server_list_p list,
                        const geoip& gi,
                        const QString& caption = QString());

    void start();
    void cancel();
    int get_progress();

private slots:
    void stopped();

private:
    void clear_updating();

    server_id_list selection_;
    server_list_updater* updater_;
};

#endif	/* _JOB_UPDATE_SELECTED_H */

