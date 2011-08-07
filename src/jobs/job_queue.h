#ifndef _JOB_QUEUE_H
#define	_JOB_QUEUE_H

#include <QList>
#include <QObject>

#include "job.h"
#include "pointers.h"

typedef QList<job_p> jobs_t;

class job_queue : public QObject
{
    Q_OBJECT
public:
    job_queue(QObject* parent = 0);

    // add job to queue
    void add_job(job_p j, bool start = true);

    // get job list
    const jobs_t& get_jobs() const {return jobs_;}

signals:
    void changed();

private slots:
    void job_state_changed(job_t::state_t);
    void clear_stopped();

private:
    jobs_t jobs_;
};

#endif

