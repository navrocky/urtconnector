#ifndef _JOB_QUEUE_H
#define	_JOB_QUEUE_H

#include <vector>

#include <QObject>
#include <QTimer>

#include "job.h"

typedef std::vector<job_p> jobs_t;

class job_queue : public QObject
{
    Q_OBJECT
public:
    job_queue(QObject* parent = 0);

    // add job to queue
    void add_job(job_p j);

    // get job list
    const jobs_t& get_jobs();

signals:
    void job_added(job_weak_p);

private slots:
    void job_state_changed(job_t::state_t);
    void clear_stopped();

private:
    void try_execute();
    void request_clear_stopped();

    jobs_t jobs_;
    QTimer clear_timer_;
};

#endif

