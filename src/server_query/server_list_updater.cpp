#include "server_list_updater.h"

#include <QTimer>
#include <boost/make_shared.hpp>
#include <common/server_list.h>
#include <common/qt_syslog.h>
#include <common/tools.h>
#include "urt_query.h"

SYSLOG_MODULE(server_list_updater)

namespace
{
const int c_time_between_info_and_status = 100;
const int c_max_sim = 0;
const int c_timeout = 2000;
const int c_retries = 2;
}

server_list_updater::server_list_updater(server_list_p list,
                                         const geoip &gi,
                                         QObject *parent)
    : QObject(parent)
    , serv_list_(list)
    , geoip_(gi)
    , dispatcher_(new urt_query_dispatcher(this))
    , maximum_sim_queries_(c_max_sim)
    , timeout_(c_timeout)
    , retries_(c_retries)
{
}

void server_list_updater::refresh_selected(const server_id_list &list)
{
    clear();

    count_ = list.size();
    id_list_ = list;
    current_id_ = 0;
    sim_queries_count_ = 0;

    query_portion();
}


void server_list_updater::clear()
{
    progress_ = 0;
    count_ = 0;
    id_list_.clear();
    current_id_ = 0;
    sim_queries_count_ = 0;
    canceled_ = false;
}

void server_list_updater::query_portion()
{
    int cnt = 0;
    while ((maximum_sim_queries_ == 0 || sim_queries_count_ < maximum_sim_queries_)
           && current_id_ < id_list_.size())
    {
        server_id id = id_list_[current_id_];

        Q_ASSERT(!queries_.contains(id));

        server_rec rec;

        urt_query_options opts;
        opts.timeout = timeout_;
        opts.retries = retries_;

        rec.info_query = new urt_get_server_info(this);
        rec.info_query->set_addr(id);
        rec.info_query->set_opts(opts);
        connect(rec.info_query, SIGNAL(finished()), SLOT(query_finished()));
        connect(rec.info_query, SIGNAL(error(QString)), SLOT(query_finished()));

        rec.status_query = new urt_get_server_status(this);
        rec.status_query->set_addr(id);
        rec.status_query->set_opts(opts);
        connect(rec.status_query, SIGNAL(finished()), SLOT(query_finished()));
        connect(rec.status_query, SIGNAL(error(QString)), SLOT(query_finished()));

        server_recs_t::iterator it = queries_.find(id);
        Q_ASSERT(it == queries_.end());
        queries_[id] = rec;

        dispatcher_->exec_query(rec.info_query);
        later_status_start_.append(id);

        current_id_++;
        sim_queries_count_++;
        cnt++;
    }
    if (cnt > 0)
    {
        QTimer::singleShot(c_time_between_info_and_status, this, SLOT(start_later()));
        LOG_DEBUG << "Created %1 queries", cnt;
    }
}

void server_list_updater::query_finished()
{
    urt_query* q = static_cast<urt_query*>(sender());
    server_id id = q->addr();

    server_recs_t::iterator it = queries_.find(id);
    Q_ASSERT(it != queries_.end());

    const server_rec& rec = it.value();
    if (rec.info_query->status() == urt_query::s_not_started ||
        rec.status_query->status() == urt_query::s_not_started)
        return;

    bool failed = rec.info_query->status() == urt_query::s_error ||
            rec.status_query->status() == urt_query::s_error;

    server_info_p si = boost::make_shared<server_info>();
    si->id = id;

    if (failed)
    {
        // query failed (timeout)
        si->status = server_info::s_down;
    }
    else
    {
        // query success
        si->status = server_info::s_up;
        si->name = q3stripcolor(rec.info_query->info()["hostname"]);
        si->game_type = rec.status_query->info()["gamename"];
        si->max_player_count = rec.info_query->info()["sv_maxclients"].toInt();
        si->mode = server_info::game_mode(rec.info_query->info()["gametype"].toInt() + 1);
        si->map = rec.info_query->info()["mapname"];
        si->map_url = rec.status_query->info()["sv_dlURL"];
        si->ping = q->ping();
        si->retries = q->retries();

        foreach (const urt_get_server_status::player_t& player, rec.status_query->players())
        {
            player_info pi;
            pi.set_nick_name(player.name);
            pi.set_ping(player.ping);
            pi.set_score(player.score);
            si->players.append(pi);
        }

        const urt_server_info_t& info = rec.status_query->info();
        urt_server_info_t::const_iterator it = info.begin();
        for (; it != info.end(); it++)
        {
            si->info[it.key()] = it.value();
        }
    }

    prepare_info(si);

    server_info_p si2 = serv_list_->get(si->id);
    if ( !si2 )
    {
        si2 = boost::make_shared<server_info>();
        si2->id = si->id;
        serv_list_->add(si2);
    }
    si2->update_from(*si);
    si2->updating = false;

    progress_++;

    LOG_HARD << "Received server info: %1, %2, ", si2->id.address(),
        si2->name;

    // remove queries
    rec.info_query->deleteLater();
    rec.status_query->deleteLater();
    queries_.erase(it);
    sim_queries_count_--;

    serv_list_->state_changed();

    query_portion();

    if (queries_.size() == 0)
    {
        // finished
        emit refresh_stopped();
    }
}

void server_list_updater::start_later()
{
    if (later_status_start_.isEmpty())
        return;

    foreach (const server_id& id, later_status_start_)
    {
        server_recs_t::iterator it = queries_.find(id);
        if (it == queries_.end())
            continue;
        server_rec& rec = it.value();
        dispatcher_->exec_query(rec.status_query);
    }
    later_status_start_.clear();
}

void server_list_updater::prepare_info(server_info_p si)
{
    if (si->status == server_info::s_down)
        si->mode = server_info::gm_none;

    si->country = geoip_.country( si->id.ip_or_host() );
    si->country_code = geoip_.code( si->id.ip_or_host() ).toLower();

    // move offline servers to bottom
    if (si->status != server_info::s_up)
        si->ping = 9999;
}

void server_list_updater::refresh_cancel()
{
    id_list_.clear();

    foreach (const server_rec& rec, queries_)
    {
        delete rec.info_query;
        delete rec.status_query;
    }
    queries_.clear();
    emit refresh_stopped();
}


