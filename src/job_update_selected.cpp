#include "job_update_selected.h"
#include "serverlistcustom.h"
#include "qstat_updater.h"
#include "qstat_options.h"

job_update_selected::job_update_selected(const server_id_list& selection, 
                                         serv_list_custom* list, qstat_options* opts)
: caption_(tr("Update selected servers"))
, selection_(selection)
, updater_(new qstat_updater(list, opts))
{
    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
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
