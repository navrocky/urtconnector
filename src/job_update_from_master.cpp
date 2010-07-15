
#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include "server_list.h"
#include "qstat_updater.h"
#include "qstat_options.h"

#include "job_update_from_master.h"

job_update_from_master::job_update_from_master( server_list_p list, const geoip& gi, qstat_options* opts)
: caption_(tr("Update from master server"))
, updater_(new qstat_updater(list, gi, opts))
{
    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));

    BOOST_FOREACH( server_info_list::value_type& info, list->list() )
    {
        info.second->fresh = false;
    }
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

