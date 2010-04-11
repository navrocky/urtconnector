#ifndef _JOB_UPDATE_SELECTED_H
#define	_JOB_UPDATE_SELECTED_H

#include <QString>

#include "jobs/job.h"

class QTimer;

class job_update_selected : public job_t
{
    Q_OBJECT
public:
    job_update_selected();

    QString get_caption();
    void start();
    void cancel();
    int get_progress();

private slots:
    void timeout();

private:
    QString caption_;
    QTimer* timer_;
    int progress_;
};

#endif	/* _JOB_UPDATE_SELECTED_H */

