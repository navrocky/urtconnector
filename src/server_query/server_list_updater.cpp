#include "server_list_updater.h"

#include <boost/make_shared.hpp>
#include <common/server_list.h>
#include <common/qt_syslog.h>
#include "urt_query.h"

SYSLOG_MODULE(server_list_updater)

server_list_updater::server_list_updater(server_list_p list,
                                         const geoip &gi,
                                         QObject *parent)
    : QObject(parent)
    , serv_list_(list)
    , geoip_(gi)
    , dispatcher_(new urt_query_dispatcher(this))
    , maximum_sim_queries_(100)
{

}

void server_list_updater::refresh_selected(const server_id_list &list)
{
    clear();

//    count_ = list.size();

//    foreach (const server_id& id, list)
//    {
//        server_info_p info = serv_list_->get(id);
//        if ( !info )
//        {
//            info = server_info_p( new server_info() );
//            info->id = id;
//            serv_list_->add(info);
//        }
//        info->updating = true;
//    }
//    serv_list_->state_changed();

    id_list_ = list;
    current_id_ = 0;
    sim_queries_count_ = 0;

    query_portion();
}


void server_list_updater::clear()
{
//    rd_.clear();
//    progress_ = 0;
//    count_ = 0;
    id_list_.clear();
    current_id_ = 0;
    sim_queries_count_ = 0;

//    cur_state_ = s_init;
}


void server_list_updater::query_portion()
{
    while (sim_queries_count_ < maximum_sim_queries_ && current_id_ < id_list_.size())
    {
        server_id id = id_list_[current_id_];

        Q_ASSERT(!queries_.contains(id));

        server_rec rec;

        rec.info_query = new urt_get_server_info(this);
        rec.info_query->set_addr(id);
        connect(rec.info_query, SIGNAL(finished()), SLOT(query_finished()));
        connect(rec.info_query, SIGNAL(error(QString)), SLOT(query_finished()));

        rec.status_query = new urt_get_server_status(this);
        rec.status_query->set_addr(id);
        connect(rec.status_query, SIGNAL(finished()), SLOT(query_finished()));
        connect(rec.status_query, SIGNAL(error(QString)), SLOT(query_finished()));

        queries_[id] = rec;

        dispatcher_->exec_query(rec.info_query);
        dispatcher_->exec_query(rec.status_query);

        current_id_++;
        sim_queries_count_++;
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
        // query failed
        si->status = server_info::s_down;
    }
    else
    {
        // query success
        si->status = server_info::s_up;
        si->name = rec.status_query->info()["sv_hostname"];


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

//        if (clear_offline_)
//            old_list_.erase(cur_server_info_->id);

    LOG_HARD << "Received server info: %1, %2, ", si2->id.address(),
        si2->name;

    // remove queries
    rec.info_query->deleteLater();
    rec.status_query->deleteLater();
    queries_.erase(it);
    maximum_sim_queries_--;

    serv_list_->state_changed();

    query_portion();
}

void server_list_updater::prepare_info(server_info_p si)
{
    if (si->status == server_info::s_down)
        si->mode = server_info::gm_none;
    else
        si->mode = (server_info::game_mode)(si->info["gametype"].toInt() + 1);

    si->country = geoip_.country( si->id.ip_or_host() );
    si->country_code = geoip_.code( si->id.ip_or_host() ).toLower();

    // move offline servers to bottom
    if (si->status != server_info::s_up)
        si->ping = 9999;
}


