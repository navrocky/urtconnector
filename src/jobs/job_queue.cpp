#include <algorithm>

#include "job_queue.h"

job_queue::job_queue(QObject* parent)
: QObject(parent)
{
    connect(&clear_timer_, SIGNAL(timeout()), SLOT(clear_stopped()));
    clear_timer_.setSingleShot(true);
}

void job_queue::add_job(job_p job)
{
    jobs_.push_back(job);
    connect(job.get(), SIGNAL(state_changed(job_t::state_t)),
            SLOT(job_state_changed(job_t::state_t)));
    try_execute();
}

void job_queue::request_clear_stopped()
{
    clear_timer_.start(500);
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
}

void job_queue::job_state_changed(job_t::state_t state)
{
    if (job_t::state_is_stopped(state))
    {
        try_execute();
        request_clear_stopped();
    }
}

void job_queue::try_execute()
{
    job_p j = get_current_job().lock();
    if (j) return;

    for (jobs_t::iterator it(jobs_.begin()); it != jobs_.end(); it++)
    {
        job_p job = *it;
        job_t::state_t state = job->get_state();

        if (state == job_t::js_not_started)
        {
            job->start();
            return;
        }
    }
}

job_weak_p job_queue::get_current_job()
{
    for (jobs_t::iterator it(jobs_.begin()); it != jobs_.end(); it++)
    {
        job_p job = *it;
        job_t::state_t state = job->get_state();
        if (state == job_t::js_executing)
            return job;
    }
    return job_weak_p();
}