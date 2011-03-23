#include <common/qt_syslog.h>
#include <common/exception.h>
#include <common/server_id.h>
#include <common/server_list.h>

#include "qstat_updater.h"

SYSLOG_MODULE(qstat_updater)

namespace
{

const char* c_qstat = "qstat";
const char* c_server = "server";
const char* c_server_type = "type";
const char* c_server_servers = "servers";
const char* c_server_status = "status";
const char* c_hostname = "hostname";
const char* c_name = "name";
const char* c_gametype = "gametype";
const char* c_map = "map";
const char* c_numplayers = "numplayers";
const char* c_maxplayers = "maxplayers";
const char* c_ping = "ping";
const char* c_retries = "retries";
const char* c_rules = "rules";
const char* c_rule = "rule";
const char* c_rule_name = "name";
const char* c_players = "players";
const char* c_player = "player";
const char* c_player_name = "name";
const char* c_player_score = "score";
const char* c_player_ping = "ping";

}

qstat_updater::qstat_updater(server_list_p list, const geoip& gi)
: cur_state_(s_init)
, cur_server_info_( new server_info() )
, gi_(gi)
, serv_list_(list)
, count_(0)
, progress_(0)
, canceled_(false)
, clear_offline_(false)
{
    connect(&proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
    connect(&proc_, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int,QProcess::ExitStatus)));
    connect(&proc_, SIGNAL(readyReadStandardOutput()), SLOT(ready_read_output()));
}

void qstat_updater::clear()
{
    rd_.clear();
    progress_ = 0;
    count_ = 0;
    cur_state_ = s_init;
}

void qstat_updater::set_clear_offline(bool val)
{
    clear_offline_ = val;
}

void qstat_updater::refresh_all()
{
    if (proc_.state() != QProcess::NotRunning) return;
    clear();

    if (clear_offline_)
        old_list_ = serv_list_->list();

    // setup updating state
    foreach (server_info_list::const_reference r, serv_list_->list())
    {
        r.second->updating = true;
    }
    serv_list_->state_changed();

    QStringList sl;
#ifdef QSTAT_FAKE
    sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    proc_.start("/bin/bash", sl);
#else
    qstat_settings qs;
    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml";// << "-retry" << "10";
    sl << "-q3m" << qs.master_server();
    proc_.start(qs.qstat_path(), sl);
#endif
}

void qstat_updater::refresh_selected(const server_id_list& list)
{
    if (proc_.state() != QProcess::NotRunning) return;
    clear();

    count_ = list.size();

    foreach (const server_id& id, list)
    {
        server_info_p info = serv_list_->get(id);
        if ( !info )
        {
            info = server_info_p( new server_info() );
            info->id = id;
            serv_list_->add(info);
        }
        info->updating = true;
    }
    serv_list_->state_changed();

    QStringList sl;

#ifdef QSTAT_FAKE
    //sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    sl << "-c" << "sleep 1; cat ../doc/bug1.txt | awk '{print $0;}'";
    proc_.start("/bin/bash", sl);
#else
    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml" << "-retry" << "10";

    for (server_id_list::const_iterator it = list.begin(); it != list.end(); it++)
        sl << "-q3s" << it->address();

    proc_.start(qstat_settings().qstat_path(), sl);
#endif
}

void qstat_updater::do_refresh_stopped()
{
    LOG_DEBUG << "Refresh stopped";
    
    // clear updating state
    const server_info_list& il = serv_list_->list();
    for (server_info_list::const_iterator it = il.begin(); it != il.end(); it++)
    {
        it->second->updating = false;
    }
    serv_list_->state_changed();

    if (clear_offline_)
    {
        LOG_DEBUG << "Removing offline";
        server_id_list l;
        foreach(server_info_list::const_reference r, old_list_)
        {
            l.push_back(r.first);
        }
        serv_list_->remove_selected(l);
    }
    
    emit refresh_stopped();
}

void qstat_updater::refresh_cancel()
{
    if (proc_.state() == QProcess::NotRunning) return;
    canceled_ = true;
    proc_.kill();
    clear();
}

void qstat_updater::error(QProcess::ProcessError error)
{
    do_refresh_stopped();
    if (canceled_) return;
    switch (error)
    {
        case QProcess::FailedToStart:
            throw qexception(tr("QStat failed to start. Check QStat binary path in options dialog."));
        case QProcess::Crashed:
            throw qexception(tr("QStat crashed"));
        case QProcess::Timedout:
            throw qexception(tr("QStat timed out"));
        case QProcess::ReadError:
            throw qexception(tr("QStat read error"));
        case QProcess::WriteError:
            throw qexception(tr("QStat write error"));
        case QProcess::UnknownError:
            throw qexception(tr("QStat unknown error"));
    }
    throw qexception(tr("QStat unknown error"));
}

void qstat_updater::finished(int, QProcess::ExitStatus)
{
    LOG_HARD << "QStat output: %1", qstat_output_;
    do_refresh_stopped();
}

void qstat_updater::ready_read_output()
{
    QByteArray a = proc_.readAll();
    qstat_output_.append(a);
    rd_.addData(a);

    while (!rd_.atEnd() && !canceled_)
    {
        if (rd_.readNext() != QXmlStreamReader::Invalid)
            process_xml();
        else
        if (rd_.hasError() && (rd_.error() != QXmlStreamReader::PrematureEndOfDocumentError)
            && !canceled_)
            throw qexception(rd_.errorString());
    }
}

void qstat_updater::process_xml()
{
    if (rd_.isStartElement())
    {
        if (cur_state_ == s_init && rd_.name() == c_qstat)
            cur_state_ = s_qstat;

        else if (cur_state_ == s_qstat && rd_.name() == c_server)
        {
            if (rd_.attributes().value(c_server_type) == "Q3M")
            {
                cur_state_ = s_master_server;
                count_ = rd_.attributes().value(c_server_servers).toString().toInt();
            } else
            {
                if (rd_.attributes().value(c_server_status) == "UP")
                    cur_server_info_->status = server_info::s_up;
                else if (rd_.attributes().value(c_server_status) == "ERROR")
                    cur_server_info_->status = server_info::s_error;
                else
                    cur_server_info_->status = server_info::s_down;

                cur_state_ = s_server;
            }
        }
        else if (cur_state_ == s_server && rd_.name() == c_hostname)
            cur_state_ = s_host_name;

        else if (cur_state_ == s_server && rd_.name() == c_name)
            cur_state_ = s_name;

        else if (cur_state_ == s_server && rd_.name() == c_gametype)
            cur_state_ = s_game_type;

        else if (cur_state_ == s_server && rd_.name() == c_map)
            cur_state_ = s_map;

        else if (cur_state_ == s_server && rd_.name() == c_numplayers)
            cur_state_ = s_num_players;

        else if (cur_state_ == s_server && rd_.name() == c_maxplayers)
            cur_state_ = s_max_players;

        else if (cur_state_ == s_server && rd_.name() == c_ping)
            cur_state_ = s_ping;

        else if (cur_state_ == s_server && rd_.name() == c_retries)
            cur_state_ = s_retries;

        else if (cur_state_ == s_server && rd_.name() == c_rules)
            cur_state_ = s_rules;

        else if (cur_state_ == s_rules && rd_.name() == c_rule)
        {
            cur_rule_ = rule_info_t();
            cur_rule_.first = rd_.attributes().value(c_rule_name).toString();
            cur_state_ = s_rule;
        }

        else if (cur_state_ == s_server && rd_.name() == c_players)
            cur_state_ = s_players;

        else if (cur_state_ == s_players && rd_.name() == c_player)
        {
            cur_player_info_ = player_info();
            cur_state_ = s_player;
        }

        else if (cur_state_ == s_player && rd_.name() == c_player_name)
            cur_state_ = s_player_name;
        else if (cur_state_ == s_player && rd_.name() == c_player_ping)
            cur_state_ = s_player_ping;
        else if (cur_state_ == s_player && rd_.name() == c_player_score)
            cur_state_ = s_player_score;

    } else

    if (rd_.isCharacters())
    {
        if (cur_state_ == s_host_name)
            cur_server_info_->id = server_id(rd_.text().toString());
        else if (cur_state_ == s_name)
            cur_server_info_->name = rd_.text().toString().trimmed();
        else if (cur_state_ == s_game_type)
            cur_server_info_->game_type = rd_.text().toString();
        else if (cur_state_ == s_map)
            cur_server_info_->map = rd_.text().toString();
        else if (cur_state_ == s_max_players)
            cur_server_info_->max_player_count = rd_.text().toString().toInt();
        else if (cur_state_ == s_ping)
            cur_server_info_->ping = rd_.text().toString().toInt();
        else if (cur_state_ == s_retries)
            cur_server_info_->retries = rd_.text().toString().toInt();
        else if (cur_state_ == s_rule)
            cur_rule_.second = rd_.text().toString();
        else if (cur_state_ == s_player_name)
            cur_player_info_.set_nick_name(rd_.text().toString().trimmed());
        else if (cur_state_ == s_player_score)
            cur_player_info_.set_score(rd_.text().toString().toInt());
        else if (cur_state_ == s_player_ping)
            cur_player_info_.set_ping(rd_.text().toString().toInt());
    } else

    if (rd_.isEndElement())
    {
        if (rd_.name() == c_server)
        {
            if (cur_state_ == s_server)
            {
                static const QString c_ut_game_type = "q3ut4";
                if (cur_server_info_->status != server_info::s_error &&
                    !cur_server_info_->id.is_empty() && cur_server_info_->game_type == c_ut_game_type)
                {
                    prepare_info();
                    server_info_p si = serv_list_->get(cur_server_info_->id);
                    if ( !si )
                    {
                        si = server_info_p( new server_info() );
                        si->id = cur_server_info_->id;
                        serv_list_->add(si);
                    }
                    si->update_from(*cur_server_info_);
                    si->updating = false;

                    if (clear_offline_)
                        old_list_.erase(cur_server_info_->id);

                    LOG_HARD << "Received server info: %1, %2, ", si->id.address(),
                        si->name;
                }

                cur_server_info_.reset( new server_info() );
                progress_++;

                serv_list_->state_changed();
            }
            cur_state_ = s_qstat;
        }
        else if (rd_.name() == c_qstat && cur_state_ == s_qstat)
            cur_state_ = s_init;
        else if ((rd_.name() == c_hostname && cur_state_ == s_host_name) ||
                (rd_.name() == c_name && cur_state_ == s_name) ||
                (rd_.name() == c_gametype && cur_state_ == s_game_type) ||
                (rd_.name() == c_map && cur_state_ == s_map) ||
                (rd_.name() == c_numplayers && cur_state_ == s_num_players) ||
                (rd_.name() == c_maxplayers && cur_state_ == s_max_players) ||
                (rd_.name() == c_ping && cur_state_ == s_ping) ||
                (rd_.name() == c_retries && cur_state_ == s_retries) ||
                (rd_.name() == c_rules && cur_state_ == s_rules) ||
                (rd_.name() == c_players && cur_state_ == s_players) )
            cur_state_ = s_server;
        else if (rd_.name() == c_rule && cur_state_ == s_rule)
        {
            cur_server_info_->info[cur_rule_.first] = cur_rule_.second;
            cur_state_ = s_rules;
        }
        else if (rd_.name() == c_player && cur_state_ == s_player)
        {
            cur_server_info_->players.push_back(cur_player_info_);
            cur_state_ = s_players;
        }
        else if ((rd_.name() == c_player_name && cur_state_ == s_player_name) ||
                (rd_.name() == c_player_ping && cur_state_ == s_player_ping) ||
                (rd_.name() == c_player_score && cur_state_ == s_player_score))
            cur_state_ = s_player;
    }
//     throw XmlParseError();

}

void qstat_updater::prepare_info()
{
    if (cur_server_info_->status == server_info::s_down)
        cur_server_info_->mode = server_info::gm_none;
    else
        cur_server_info_->mode = (server_info::game_mode)(cur_server_info_->info["gametype"].toInt() + 1);

    cur_server_info_->country = gi_.country( cur_server_info_->id.ip_or_host() );
    cur_server_info_->country_code = gi_.code( cur_server_info_->id.ip_or_host() ).toLower();
    
    // move offline servers to bottom
    if (cur_server_info_->status != server_info::s_up)
        cur_server_info_->ping = 9999;
}

