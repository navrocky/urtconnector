#include <common/server_list.h>
#include "qstat_updater.h"

#include "job_update_selected.h"

job_update_selected::job_update_selected(const server_id_list& selection, 
                                         server_list_p list,
                                         const geoip& gi,
                                         const QString& caption_)
    : selection_(selection)
    , updater_(new qstat_updater(list, gi))
{
    set_caption(caption_.isEmpty() ? tr("Update selected servers") : caption_);
    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
}

void job_update_selected::start()
{
    set_state(job_t::js_executing);

    updater_->refresh_selected(selection_);
}

void job_update_selected::cancel()
{
    set_state(job_t::js_canceled);
    updater_->refresh_cancel();
}

int job_update_selected::get_progress()
{
    int cnt = updater_->get_count();
    int progress = updater_->get_progress();
    if (cnt > 0)
        return progress * 100 / cnt;
    else
        return 0;
}

void job_update_selected::stopped()
{
    set_state(js_finished);
}
