#include <common/exception.h>
#include <memory>

#include <QEventLoop>

#include "job.h"

job_t::job_t()
: state_(js_not_started)
{
}

void job_t::set_state(state_t state)
{
    if (state_ == state) return;
    state_ = state;
    emit state_changed(state);

    if (event_loop_ && (state_ == js_canceled || state_ == js_finished))
        event_loop_->quit();
}

bool job_t::is_stopped()
{
    return state_is_stopped(state_);
}

bool job_t::is_canceled()
{
    return state_ == js_canceled;
}

bool job_t::state_is_stopped(state_t state)
{
    return state == job_t::js_finished || state == job_t::js_canceled;
}

void job_t::wait_for_finish()
{
    if (event_loop_)
        throw qexception(tr("Can't wait a one job twice"));
    event_loop_ = new QEventLoop;
    std::auto_ptr<QEventLoop> el(event_loop_);
    event_loop_->exec();
}

void job_t::start_and_wait_for_finish()
{
    start();
    wait_for_finish();
}

