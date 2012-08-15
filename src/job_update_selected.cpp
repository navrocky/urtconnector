#include "job_update_selected.h"

#include <common/server_list.h>
#include <server_query/server_list_updater.h>
#include <common/qstat_options.h>

job_update_selected::job_update_selected(const server_id_list& selection, 
                                         server_list_p list,
                                         const geoip& gi,
                                         const QString& caption_)
    : selection_(selection)
    , updater_(new server_list_updater(list, gi, this))
{
    qstat_options opts;
    updater_->set_max_sim_queries(opts.max_sim_queries());
    updater_->set_retries(opts.retry_number());
    updater_->set_timeout(opts.timeout());
    set_caption(caption_.isEmpty() ? tr("Update selected servers") : caption_);
    connect(updater_, SIGNAL(refresh_stopped()), SLOT(stopped()));
}

void job_update_selected::start()
{
    set_state(job_t::js_executing);

    // setup updating state
    foreach (const server_id& id, selection_)
    {
        server_info_p info = updater_->server_list()->get(id);
        if ( !info )
        {
            info = server_info_p( new server_info() );
            info->id = id;
            updater_->server_list()->add(info);
        }
        info->updating = true;
    }
    updater_->server_list()->state_changed();

    updater_->refresh_selected(selection_);
}

void job_update_selected::cancel()
{
    set_state(job_t::js_canceled);
    updater_->refresh_cancel();
    clear_updating();
}

int job_update_selected::get_progress()
{
    int cnt = updater_->count();
    int progress = updater_->progress();
    if (cnt > 0)
        return progress * 100 / cnt;
    else
        return 0;
}

void job_update_selected::stopped()
{
    set_state(js_finished);
    clear_updating();
}

void job_update_selected::clear_updating()
{
    foreach (const server_id& id, selection_)
    {
        server_info_p info = updater_->server_list()->get(id);
        if ( !info )
            continue;
        info->updating = false;
    }
    updater_->server_list()->state_changed();
}
