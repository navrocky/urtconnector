#include "exception.h"
#include "server_id.h"
#include "server_list.h"

#include "qstat_updater.h"

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

qstat_updater::qstat_updater(server_list_p list, const geoip& gi, qstat_options* opts)
: cur_state_(s_init)
, gi_(gi)
, qstat_opts_(opts)
, serv_list_(list)
, count_(0)
, progress_(0)
, canceled_(false)
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

void qstat_updater::refresh_all()
{
    if (proc_.state() != QProcess::NotRunning) return;
    clear();

    QStringList sl;
#ifdef QSTAT_FAKE
    sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    proc_.start("/bin/bash", sl);
#else
    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml";// << "-retry" << "10";
    sl << "-q3m" << qstat_opts_->master_server;
    proc_.start(qstat_opts_->qstat_path, sl);
#endif
}

void qstat_updater::refresh_selected(const server_id_list& list)
{
    if (proc_.state() != QProcess::NotRunning) return;
    clear();

    count_ = list.size();

    server_info_list& info_list = serv_list_->list();

    for (server_id_list::const_iterator it = list.begin(); it != list.end(); it++)
    {
        server_id id = it->address();
        server_info info = info_list[id];
        info.status = server_info::s_updating;
        info_list[id] = info;
    }
    serv_list_->change_state();

    QStringList sl;

#ifdef QSTAT_FAKE
    //sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    sl << "-c" << "sleep 1; cat ../doc/bug1.txt | awk '{print $0;}'";
    proc_.start("/bin/bash", sl);
#else
    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml" << "-retry" << "10";

    for (server_id_list::const_iterator it = list.begin(); it != list.end(); it++)
        sl << "-q3s" << it->address();

    proc_.start(qstat_opts_->qstat_path, sl);
#endif
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
    emit refresh_stopped();
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
    emit refresh_stopped();
}

void qstat_updater::ready_read_output()
{
    rd_.addData(proc_.readAll());

    while (!rd_.atEnd())
    {
        if (rd_.readNext() != QXmlStreamReader::Invalid)
            process_xml();
        else

        if (rd_.hasError() && (rd_.error() != QXmlStreamReader::PrematureEndOfDocumentError))
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
                cur_server_info_ = server_info();
                if (rd_.attributes().value(c_server_status) == "UP")
                    cur_server_info_.status = server_info::s_up;
                else
                    cur_server_info_.status = server_info::s_down;

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
            cur_server_info_.id = server_id(rd_.text().toString());
        else if (cur_state_ == s_name)
            cur_server_info_.name = rd_.text().toString();
        else if (cur_state_ == s_game_type)
            cur_server_info_.game_type = rd_.text().toString();
        else if (cur_state_ == s_map)
            cur_server_info_.map = rd_.text().toString();
        else if (cur_state_ == s_max_players)
            cur_server_info_.max_player_count = rd_.text().toString().toInt();
        else if (cur_state_ == s_ping)
            cur_server_info_.ping = rd_.text().toString().toInt();
        else if (cur_state_ == s_retries)
            cur_server_info_.retries = rd_.text().toString().toInt();
        else if (cur_state_ == s_rule)
            cur_rule_.second = rd_.text().toString();
        else if (cur_state_ == s_player_name)
            cur_player_info_.nick_name = rd_.text().toString();
        else if (cur_state_ == s_player_score)
            cur_player_info_.score = rd_.text().toString().toInt();
        else if (cur_state_ == s_player_ping)
            cur_player_info_.ping = rd_.text().toString().toInt();
    } else

    if (rd_.isEndElement())
    {
        if (rd_.name() == c_server)
        {
            if (cur_state_ == s_server)
            {
                prepare_info();
                server_info_list& list = serv_list_->list();
                server_info& old_si = list[cur_server_info_.id];
                cur_server_info_.update_stamp = old_si.update_stamp + 1;
                //FIXME Тут в список server_info вставляется новый элемент, который об уже проинициализированном geoip ничего не знает.
                // Думаю надо в server_list_widget сделать функцию update_server_info(id) и все действия по замене server_info проводить в нем
                list[cur_server_info_.id] = cur_server_info_;
                progress_++;

                serv_list_->change_state();
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
            cur_server_info_.info[cur_rule_.first] = cur_rule_.second;
            cur_state_ = s_rules;
        }
        else if (rd_.name() == c_player && cur_state_ == s_player)
        {
            cur_server_info_.players.push_back(cur_player_info_);
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
    if (cur_server_info_.status == server_info::s_down)
        cur_server_info_.mode = server_info::gm_none;
    else
        cur_server_info_.mode = (server_info::game_mode)(cur_server_info_.info["gametype"].toInt() + 1);
    cur_server_info_.country = gi_.country( cur_server_info_.id.ip() );
    cur_server_info_.country_code = gi_.code( cur_server_info_.id.ip() ).toLower();
    cur_server_info_.country_flag = QIcon( QString(":/flags/flags/%1.png").arg( gi_.code(cur_server_info_.id.ip()).toLower() ) );
}

