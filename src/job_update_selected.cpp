#include <QTimer>

#include "job_update_selected.h"

job_update_selected::job_update_selected()
: caption_(tr("Update selected servers"))
, progress_(0)
{
    timer_ = new QTimer(this);
    timer_->setInterval(100);
    connect(timer_, SIGNAL(timeout()), SLOT(timeout()));
}

QString job_update_selected::get_caption()
{
    return caption_;
}

void job_update_selected::start()
{
    set_state(job_t::js_executing);
    timer_->start();
}

void job_update_selected::cancel()
{
    set_state(job_t::js_canceled);
    timer_->stop();
}

int job_update_selected::get_progress()
{
    return progress_;
}

void job_update_selected::timeout()
{
    progress_ += 5;
    if (progress_ > 100)
    {
        progress_ = 100;
        set_state(job_t::js_finished);
        timer_->stop();
    }
}
