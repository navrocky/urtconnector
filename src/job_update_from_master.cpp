
#include <common/server_list.h>
#include "qstat_updater.h"
#include "app_options.h"

#include "job_update_from_master.h"

job_update_from_master::job_update_from_master( server_list_p list, const geoip& gi)
: caption_(tr("Update from master server"))
, updater_(new qstat_updater(list, gi))
{
    app_settings as;
    updater_->set_clear_offline(as.clear_offline());

    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
}

job_update_from_master::~job_update_from_master()
{}

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

