
#include <common/server_list.h>
#include "qstat_updater.h"
#include "qstat_options.h"

#include "job_update_selected.h"

job_update_selected::job_update_selected(const server_id_list& selection, 
                                         server_list_p list,
                                         const geoip& gi,
                                         const QString& caption)
: caption_(tr("Update selected servers"))
, selection_(selection)
, updater_(new qstat_updater(list, gi))
{
    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
    if (!caption.isEmpty())
        caption_ = caption;
}

QString job_update_selected::get_caption()
{
    return caption_;
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
