
#include <common/server_list.h>
#include <common/tools.h>
#include <common/qt_syslog.h>
#include "qstat_updater.h"
#include "app_options.h"
#include "common/qstat_options.h"

#include "job_update_from_master.h"

SYSLOG_MODULE(job_update_from_master)

job_update_from_master::job_update_from_master( server_list_p list, const geoip& gi)
    : updater_(new qstat_updater(list, gi))
    , dispatcher_(new urt_query_dispatcher(this))
    , get_list_query_(0)
{
    set_caption(tr("Update from master server"));
    app_settings as;
    updater_->set_clear_offline(as.clear_offline());

    connect(updater_.get(), SIGNAL(refresh_stopped()), SLOT(stopped()));
}

job_update_from_master::~job_update_from_master()
{}

void job_update_from_master::start()
{
    set_state(job_t::js_executing);

    qstat_options opts;
    QString address = opts.master_server();

    get_list_query_ = new urt_get_server_list(this);
    get_list_query_->set_addr(server_id(address, 27950));
    connect(get_list_query_, SIGNAL(finished(server_id_list)), SLOT(query_server_list_finished(server_id_list)));
    connect(get_list_query_, SIGNAL(error(QString)), SLOT(error(QString)));
    dispatcher_->exec_query(get_list_query_);

//    updater_->refresh_all();
}

void job_update_from_master::cancel()
{
    set_state(job_t::js_canceled);
    delete get_list_query_;
    get_list_query_ = 0;

//    updater_->refresh_cancel();
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

void job_update_from_master::query_server_list_finished(const server_id_list & list)
{
    LOG_DEBUG << "Taken %1 servers from master", list.size();
    delete get_list_query_;
    get_list_query_ = 0;
    set_state(js_finished);
}

void job_update_from_master::error(const QString &msg)
{
    delete get_list_query_;
    get_list_query_ = 0;
    set_state(js_finished);
    show_error(msg);
}

