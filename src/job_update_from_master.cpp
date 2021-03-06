#include "job_update_from_master.h"

#include <common/server_list.h>
#include <common/tools.h>
#include <common/qt_syslog.h>
#include <common/qstat_options.h>
#include <server_query/server_list_updater.h>

#include "app_options.h"

SYSLOG_MODULE(job_update_from_master)

job_update_from_master::job_update_from_master( server_list_p list, const geoip& gi)
    : dispatcher_(new urt_query_dispatcher(this))
    , get_list_query_(0)
{
    set_caption(tr("Update from master server"));
    qstat_options opts;

    updater_ = new server_list_updater(list, gi, this);
    updater_->set_max_sim_queries(opts.max_sim_queries());
    updater_->set_retries(opts.retry_number());
    updater_->set_timeout(opts.timeout());
    connect(updater_, SIGNAL(refresh_stopped()), SLOT(stopped()));
}

job_update_from_master::~job_update_from_master()
{}

void job_update_from_master::start()
{
    set_state(job_t::js_executing);

    app_settings as;
    clear_offline_ = as.clear_offline();

    qstat_options qo;
    QString address = qo.master_server();

    urt_query_options opts;
    opts.retries = qo.retry_number();
    opts.timeout = qo.timeout();
    opts.series_timeout = qo.series_timeout();

    get_list_query_ = new urt_get_server_list(this);
    get_list_query_->set_addr(server_id(address, 27950));
    get_list_query_->set_opts(opts);

    connect(get_list_query_, SIGNAL(finished(server_id_list)), SLOT(query_server_list_finished(server_id_list)));
    connect(get_list_query_, SIGNAL(error(QString)), SLOT(error(QString)));
    dispatcher_->exec_query(get_list_query_);

    // setup updating state
    foreach (server_info_list::const_reference r, updater_->server_list()->list())
    {
        r.second->updating = true;
    }
    updater_->server_list()->state_changed();
}

void job_update_from_master::cancel()
{
    set_state(job_t::js_canceled);
    delete get_list_query_;
    get_list_query_ = 0;

    // setup updating state
    foreach (server_info_list::const_reference r, updater_->server_list()->list())
    {
        r.second->updating = false;
    }
    updater_->server_list()->state_changed();

    delete updater_;
    updater_ = 0;
}

int job_update_from_master::get_progress()
{
    int cnt = updater_->count();
    int progress = updater_->progress();
    if (cnt > 0)
        return progress * 100 / cnt;
    else
        return 0;
}

void job_update_from_master::stopped()
{
    server_list_p l = updater_->server_list();
    if (clear_offline_)
    {
        server_id_list to_remove;
        foreach (server_info_list::const_reference r, l->list())
        {
            server_info_p si = r.second;
            if (si->updating || si->status == server_info::s_down || si->status == server_info::s_error)
                to_remove.append(r.first);
        }
        l->remove_selected(to_remove);
    } else
    {
        foreach (server_info_list::const_reference r, l->list())
            r.second->updating = false;
        updater_->server_list()->state_changed();
    }

    set_state(js_finished);
}

void job_update_from_master::query_server_list_finished(const server_id_list & list)
{
    LOG_DEBUG << "Taken %1 servers from master", list.size();
    updater_->refresh_selected(list);
}

void job_update_from_master::error(const QString &msg)
{
    delete get_list_query_;
    get_list_query_ = 0;
    set_state(js_finished);
    show_error(msg);
}

