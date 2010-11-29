#include <stdexcept>

#include <QCoreApplication>
#include <QSettings>
#include <QByteArray>

#include <cl/syslog/syslog.h>

#include "pointers.h"
#include "server_list.h"

#include "server_list_saver.h"

SYSLOG_MODULE(server_list_saver)

void save_server_info(qsettings_p s, const server_info_p& info)
{
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
        const server_info_list& l = list->list();
        s->beginWriteArray(name);
        int i = 0;
        for (server_info_list::const_iterator it = l.begin(); it != l.end(); it++)
        {
            s->setArrayIndex(i++);
            save_server_info(s, it->second);
        }
        s->endArray();
    }
    catch(...)
    {
        LOG_ERR << "Error occured while saving server list";
    }
}

void load_server_list(qsettings_p s, const QString& name, server_list_p list)
{
    LOG_DEBUG << "Loading server list \"%1\"", s->fileName().toStdString();
    try
    {
        server_info_list& l = list->list();
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
        s->endArray();
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
