#include <QCoreApplication>
#include <QSettings>
//#include <QByteArray>
//#include <QFile>

#include "pointers.h"
#include "server_list_saver.h"

qsettings_p get_server_list_settings(const QString& name)
{
    return qsettings_p(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                     qApp->organizationName(), qApp->applicationName() + "_" + name));
}

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

void save_server_list(qsettings_p s, const QString& name, const server_list& list)
{
    try
    {
        const server_info_list& l = list.list();
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
    {}
}

void load_server_list(qsettings_p s, const QString& name, server_list& list)
{
    try
    {
        server_info_list& l = list.list();
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
