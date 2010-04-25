#include "serverlistcustom.h"
#include "qstat_updater.h"
#include "qstat_options.h"

#include "job_update_from_master.h"

job_update_from_master::job_update_from_master(serv_list_custom* list, qstat_options* opts)
: caption_(tr("Update from master server"))
, updater_(new qstat_updater(list, opts))
{
    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
}

QString job_update_from_master::get_caption()
{
    return caption_;
}

void job_update_from_master::start()
{
    set_state(job_t::js_executing);
    updater_->refresh_all();
}

void job_update_from_master::cancel()
{
    set_state(job_t::js_canceled);
    updater_->refresh_cancel();
}

int job_update_from_master::get_progress()
{
    int cnt = updater_->get_count();
    int progress = updater_->get_progress();
    if (cnt > 0)
        return progress * 100 / cnt;
    else
        return 0;
}

void job_update_from_master::stopped()
{
    set_state(js_finished);
}

