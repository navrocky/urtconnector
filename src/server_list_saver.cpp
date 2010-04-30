#include <QSettings>

#include "server_list_saver.h"
#include "app_options_saver.h"

void save_server_info(qsettings_p s, const server_info& info)
{
    s->setValue("address", info.id.address());
    s->setValue("name", info.name);
    s->setValue("game_type", info.game_type);
    s->setValue("map", info.map);
    s->setValue("map_url", info.map_url);
    s->setValue("max_player_count", info.max_player_count);
    s->setValue("mode", info.mode);
    s->setValue("ping", info.ping);
    s->beginWriteArray("info");
    const server_info::info_t& inf = info.info;
    int i = 0;
    for (server_info::info_t::const_iterator it = inf.begin(); it != inf.end(); it++)
    {
        s->setArrayIndex(i++);
        s->setValue("key", it->first);
        s->setValue("value", it->second);
    }
    s->endArray();
}

void load_server_info(qsettings_p s, server_info& info)
{
    info.id = server_id( s->value("address").toString());
    info.name = s->value("name").toString();
    info.game_type = s->value("game_type").toInt();
    info.map = s->value("map").toString();
    info.map_url = s->value("map_url").toString();
    info.max_player_count = s->value("max_player_count").toInt();
    info.mode = static_cast<server_info::game_mode>(s->value("mode").toInt());
    info.ping = s->value("ping").toInt();
    int size = s->beginReadArray("info");
    server_info::info_t& inf = info.info;
    for (int i = 0; i < size; i++)
    {
        s->setArrayIndex(i);
        inf[s->value("key").toString()] = s->value("value").toString();
    }
    s->endArray();
}

void save_server_list(qsettings_p s, const QString& name, const server_list& list)
{
    const server_info_list& l = list.list();
    s->beginWriteArray(name);
    int i = 0;
    for (server_info_list::const_iterator it = l.begin(); it != l.end(); it++)
    {
        s->setArrayIndex(i++);
        const server_info& info = it->second;
        save_server_info(s, info);
    }
    s->endArray();
}

void load_server_list(qsettings_p s, const QString& name, server_list& list)
{
    server_info_list& l = list.list();
    int size = s->beginReadArray(name);
    for (int i = 0; i < size; i++)
    {
        s->setArrayIndex(i);
        server_info info;
        load_server_info(s, info);
        l[info.id] = info;
    }
    s->endArray();
}
