#include <stdexcept>

#include <QCoreApplication>
#include <QSettings>
#include <QByteArray>

#include <common/qt_syslog.h>
#include <common/exception.h>
#include <database/simple_database.h>

#include "pointers.h"
#include <common/server_list.h>

#include "server_list_saver.h"

SYSLOG_MODULE(server_list_saver)

void init_database()
{
    database* db = database::instance();

    db->query
    (
        "CREATE TABLE IF NOT EXISTS all_state"
        "("
        "    id INTEGER,"
        "    address VARCHAR(255),"
        "    name VARCHAR(255),"
        "    gametype VARCHAR(255),"
        "    map VARCHAR(255),"
        "    mapurl VARCHAR(255),"
        "    maxplayercount VARCHAR(255),"
        "    mode INTEGER,"
        "    ping INTEGER,"
        "    country VARCHAR(255),"
        "    countrycode VARCHAR(255),"
        "    info TEXT,"
        "    PRIMARY KEY(id)"
        ");"
    );
    db->query
    (
        "CREATE TABLE IF NOT EXISTS favs_state"
        "("
        "    id INTEGER,"
        "    address VARCHAR(255),"
        "    name VARCHAR(255),"
        "    gametype VARCHAR(255),"
        "    map VARCHAR(255),"
        "    mapurl VARCHAR(255),"
        "    maxplayercount VARCHAR(255),"
        "    mode INTEGER,"
        "    ping INTEGER,"
        "    country VARCHAR(255),"
        "    countrycode VARCHAR(255),"
        "    info TEXT,"
        "    PRIMARY KEY(id)"
        ");"
    );
}

void save_server_info(const QString &name, const server_info_p& info)
{
    QStringList sl;
    foreach (server_info::info_t::const_reference r, info->info)
    {
        sl << QString("%1=%2").arg(database::qqencode_string(r.first))
                .arg(database::qqencode_string(r.second));
    }
    QString db_field_info = sl.join("&");
    QString query = QString("INSERT INTO %0 VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11');")
                    .arg(name)
                    .arg(database::qqencode_string(info->id.address()))
                    .arg(database::qqencode_string(info->name))
                    .arg(database::qqencode_string(info->game_type))
                    .arg(database::qqencode_string(info->map))
                    .arg(database::qqencode_string(info->map_url))
                    .arg(info->max_player_count)
                    .arg(info->mode)
                    .arg(info->ping)
                    .arg(database::qqencode_string(info->country))
                    .arg(database::qqencode_string(info->country_code))
                    .arg(db_field_info);

    database::instance()->query(query);
}

void save_server_list(const QString& name, const server_list_p list)
{
    LOG_DEBUG << "Saving server list \"%1\"", name;
    try
    {
        database* db = database::instance();
        db->query(QString("DELETE FROM %1;").arg(name));
        db->query("BEGIN TRANSACTION;");
        foreach (server_info_list::const_reference r, list->list())
        {
            save_server_info(name, r.second);
        }
        db->query("END TRANSACTION;");
    }
    catch(const std::exception& ex)
    {
        LOG_ERR << "Error occured while saving server list: %1", ex.what();
    }
}

void load_server_list(const QString& name, server_list_p list)
{
    LOG_DEBUG << "Loading server list \"%1\"", name;
    try
    {
        database::result_set servers = database::instance()->query(QString("SELECT * FROM %1;").arg(name));
        foreach (const database::result_row& row, servers)
        {
            server_info_p info(new server_info());
            info->id = server_id(database::qqdecode_string(row[1]));
            info->name = database::qqdecode_string(row[2]);
            info->game_type = database::qqdecode_string(row[3]);
            info->map = database::qqdecode_string(row[4]);
            info->map_url = database::qqdecode_string(row[5]);
            info->max_player_count = database::qqdecode_string(row[6]).toInt();
            info->mode = static_cast<server_info::game_mode>(database::qqdecode_string(row[7]).toInt());
            info->ping = database::qqdecode_string(row[8]).toInt();
            info->country = database::qqdecode_string(row[9]);
            info->country_code = database::qqdecode_string(row[10]);

            server_info::info_t& inf = info->info;

            QString d_inf = row[11];
            QStringList sl = d_inf.split("&");
            QRegExp rx("^(.+)=(.+)$");
            foreach (const QString& s, sl)
            {
                if (rx.exactMatch(s))
                    inf[database::qqdecode_string(rx.cap(1))] =
                        database::qqdecode_string(rx.cap(2));
            }
            list->add(info);
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERR << "Error occured while loading server list: %1", e.what();
    }
}

