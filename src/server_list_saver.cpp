#include <stdexcept>

#include <QCoreApplication>
#include <QSettings>
#include <QByteArray>

#include <cl/syslog/syslog.h>
#include <common/exception.h>
#include "pointers.h"
#include "server_list.h"

#include "server_list_saver.h"
#include <database/simple_database.h>

SYSLOG_MODULE(server_list_saver)

void save_server_info(const QString &name, qsettings_p s, const server_info_p& info)
{
    QString db_field_info = "";
    const server_info::info_t& inf = info->info;
    for (server_info::info_t::const_iterator it = inf.begin(); it!=inf.end(); ++it)
    {
        db_field_info += simple_database::db->qqencode_string(it->first);
        db_field_info += " = ";
        db_field_info += simple_database::db->qqencode_string(it->second);
        db_field_info += " & ";
    }
    QString query = QString("INSERT INTO %0 VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11');")
    .arg(name)
    .arg(simple_database::db->qqencode_string(info->id.address()))
    .arg(simple_database::db->qqencode_string(info->name))
    .arg(simple_database::db->qqencode_string(info->game_type))
    .arg(simple_database::db->qqencode_string(info->map))
    .arg(simple_database::db->qqencode_string(info->map_url))
    .arg(info->max_player_count)
    .arg(info->mode)
    .arg(info->ping)
    .arg(simple_database::db->qqencode_string(info->country))
    .arg(simple_database::db->qqencode_string(info->country_code))
    .arg(db_field_info);

    simple_database::db->query(query.toStdString());
/*
    s->setValue("address", info->id.address());
    s->setValue("name", info->name);
    s->setValue("game_type", info->game_type);
    s->setValue("map", info->map);
    s->setValue("map_url", info->map_url);
    s->setValue("max_player_count", info->max_player_count);
    s->setValue("mode", info->mode);
    s->setValue("ping", info->ping);
    s->setValue("country", info->country);
    s->setValue("country_code", info->country_code);
    s->beginWriteArray("info");
    const server_info::info_t& inf = info->info;
    int i = 0;
    for (server_info::info_t::const_iterator it = inf.begin(); it != inf.end(); it++)
    {
        s->setArrayIndex(i++);
        s->setValue("key", it->first);
        s->setValue("value", it->second);
    }
    s->endArray();
*/
}

// typedef QMap<QString, QVariant> props_t;
// 
// void save_server_info2(props_t& props, const server_info_p& info)
// {
//     props["address"] = info->id.address();
//     props["name"] = info->name;
//     props["game_type"] = info->game_type;
//     props["map"] = info->map;
//     props["map_url"] = info->map_url;
//     props["max_player_count"] = info->max_player_count;
//     props["mode"] = info->mode;
//     props["ping"] = info->ping;
//     props["country"] = info->country;
//     props["country_code"] = info->country_code;
// 
//     props_t dst_info;
//     const server_info::info_t& inf = info->info;
//     for (server_info::info_t::const_iterator it = inf.begin(); it != inf.end(); it++)
//         dst_info[it->first] = it->second;
// 
//     props["info"] = dst_info;
// }
// 
// void load_server_info2(const props_t& props, server_info_p& info)
// {
//     props_t::const_iterator prop;
// 
//     prop = props.find("address");
//     if (prop != props.end())
//     {
//         LOG_HARD << "addr = %1", prop->toString().toStdString();
//         
//         info->id = server_id(prop->toString());
//     }
//     LOG_HARD << "info.id = %1", info->id.address().toStdString();
//     prop = props.find("name");
//     if (prop != props.end())
//         info->name = prop->toString();
//     LOG_HARD << "info.name = %1", info->name.toStdString();
//     prop = props.find("game_type");
//     if (prop != props.end())
//         info->game_type = prop->toString();
//     LOG_HARD << "info.game_type = %1", info->game_type.toStdString();
//     prop = props.find("map");
//     if (prop != props.end())
//         info->map = prop->toString();
//     prop = props.find("map_url");
//     if (prop != props.end())
//         info->map_url = prop->toString();
//     prop = props.find("max_player_count");
//     if (prop != props.end())
//         info->max_player_count = prop->toInt();
//     prop = props.find("mode");
//     if (prop != props.end())
//         info->mode = static_cast<server_info::game_mode>(prop->toInt());
//     prop = props.find("ping");
//     if (prop != props.end())
//         info->ping = prop->toInt();
//     prop = props.find("country");
//     if (prop != props.end())
//         info->country = prop->toString();
//     prop = props.find("country_code");
//     if (prop != props.end())
//         info->country_code = prop->toString();
//     prop = props.find("info");
//     if (prop != props.end())
//     {
//         props_t src = prop.value().value<props_t>();
//         server_info::info_t& inf = info->info;
//         inf.clear();
//         for (props_t::const_iterator it = src.begin(); it != src.end(); it++)
//             inf[it.key()] = it->toString();
//     }
// }

void load_server_info(qsettings_p s, server_info_p info)
{
    info->id = server_id( s->value("address").toString());
    info->name = s->value("name").toString();
    info->game_type = s->value("game_type").toInt();
    info->map = s->value("map").toString();
    info->map_url = s->value("map_url").toString();
    info->max_player_count = s->value("max_player_count").toInt();
    info->mode = static_cast<server_info::game_mode>(s->value("mode").toInt());
    info->ping = s->value("ping").toInt();
    info->country = s->value("country").toString();
    info->country_code = s->value("country_code").toString();
    int size = s->beginReadArray("info");
    server_info::info_t& inf = info->info;
    for (int i = 0; i < size; i++)
    {
        s->setArrayIndex(i);
        inf[s->value("key").toString()] = s->value("value").toString();
    }
    s->endArray();
}

void save_server_list(qsettings_p s, const QString& name, const server_list_p list)
{
    LOG_DEBUG << "Saving server list \"%1\"", s->fileName().toStdString();
    try
    {
        simple_database::db->query(QString("DELETE FROM %1;").arg(name).toStdString());
        simple_database::db->query("BEGIN TRANSACTION;");
        const server_info_list& l = list->list();
        int i = 0;
        for (server_info_list::const_iterator it = l.begin(); it != l.end(); it++)
        {
            save_server_info(name, s, it->second);
        }
        simple_database::db->query("END TRANSACTION;");
    }
    catch(qexception ex)
    {
        LOG_ERR << "Error occured while saving server list";
        LOG_ERR << ex.message().toStdString();
    }
}

void load_server_list(qsettings_p s, const QString& name, server_list_p list)
{
    LOG_DEBUG << "Loading server list \"%1\"", s->fileName().toStdString();
    try
    {
        server_info_list& l = list->list();
        simple_database::result_set servers = simple_database::db->query(QString("SELECT * FROM %1;").arg(name).toStdString());
        size_t rows = servers.size();
        for (size_t i=0; i<rows; ++i)
        {
            server_info_p info(new server_info());
            info->id = server_id(simple_database::db->sqdecode_string(servers[i][1]));
            info->name = simple_database::db->sqdecode_string(servers[i][2]);
            info->game_type = simple_database::db->sqdecode_string(servers[i][3]).toInt();
            info->map = simple_database::db->sqdecode_string(servers[i][4]);
            info->map_url = simple_database::db->sqdecode_string(servers[i][5]);
            info->max_player_count = simple_database::db->sqdecode_string(servers[i][6]).toInt();
            info->mode = static_cast<server_info::game_mode>(simple_database::db->sqdecode_string(servers[i][7]).toInt());
            info->ping = simple_database::db->sqdecode_string(servers[i][8]).toInt();
            info->country = simple_database::db->sqdecode_string(servers[i][9]);
            info->country_code = simple_database::db->sqdecode_string(servers[i][10]);

            server_info::info_t& inf = info->info;

            QString d_inf = servers[i][11].c_str();
            size_t len = d_inf.size();
            size_t prev = 0;
            //a = b & c = d
            for (size_t i=1; i<len; ++i)
            {
                if (d_inf[i] == '&' && d_inf[i-1] == ' ')
                {
                    size_t j = i-1;
                    for (; j>0; --j)
                    {
                        if (d_inf[j] == '=' && d_inf[j-1] == ' ')
                        {
                            break;
                        }
                    }
                    inf[simple_database::db->qqdecode_string(d_inf.mid(prev,j-prev-1))] = 
                        simple_database::db->qqdecode_string(d_inf.mid(j+2,i-j-3));
                    prev = i+2;
                }
            }
            l[info->id] = info;
        }
/*        server_info_list& l = list->list();
        int size = s->beginReadArray(name);
        for (int i = 0; i < size; i++)
        {
            s->setArrayIndex(i);
            server_info_p info( new server_info() );
            try
            {
                load_server_info(s, info);
                l[info->id] = info;
            }
            catch(...)
            {}
        }
        s->endArray();*/
    }
    catch(...)
    {}
}

// QByteArray save_server_list2(const server_list& list)
// {
//     const server_info_list& l = list.list();
//     QByteArray res;
//     QDataStream stream(&res, QIODevice::WriteOnly);
//     stream << list.list().size();
//     for (server_info_list::const_iterator it = l.begin(); it != l.end(); it++)
//     {
//         server_info_p info = it->second;
//         props_t props;
//         save_server_info2(props, info);
//         stream << props;
//     }
//     return res;
// }
// 
// void load_server_list2(server_list& list, const QByteArray& ba)
// {
//     try
//     {
//         server_info_list& l = list.list();
//         QDataStream stream(ba);
//         int size;
//         stream >> size;
//         LOG_HARD << "Loading %1 server infos...", size;
//         for (int i = 0; i < size; i++)
//         {
//             props_t props;
//             stream >> props;
//             server_info_p info(new server_info);
//             load_server_info2(props, info);
//             l[info->id] = info;
//             LOG_HARD << "Load info #%1 success", i;
//         }
//     }
//     catch(const std::exception& e)
//     {
//         LOG_ERR << "Error \"%1\"occured while loading server list", e.what();
//     }
//     catch(...)
//     {
//         LOG_ERR << "Error occured while loading server list";
//     }
// }
