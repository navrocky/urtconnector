#include "job_queue.h"

#include <algorithm>
#include <QTimer>

job_queue::job_queue(QObject* parent)
: QObject(parent)
{
}

void job_queue::add_job(job_p job, bool start)
{
    jobs_.append(job);
    connect(job.get(), SIGNAL(state_changed(job_t::state_t)),
            SLOT(job_state_changed(job_t::state_t)));
    if (start)
        job->start();
    emit changed();
}

class stopped_job
{
public:
    bool operator()(const job_p& j) const
    {
        return j->is_stopped();
    }
};

void job_queue::clear_stopped()
{
    jobs_.erase(std::remove_if(jobs_.begin(), jobs_.end(), stopped_job()), jobs_.end());
    emit changed();
}

void job_queue::job_state_changed(job_t::state_t state)
{
    if (job_t::state_is_stopped(state))
    {
        QTimer::singleShot(0, this, SLOT(clear_stopped()));
    }
    emit changed();
}
