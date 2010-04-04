
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
}

bool job_t::is_stopped()
{
    return state_is_stopped(state_);
}

bool job_t::state_is_stopped(state_t state)
{
    return state == job_t::js_finished || state == job_t::js_canceled;
}

