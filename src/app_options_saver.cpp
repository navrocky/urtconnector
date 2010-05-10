#include <QCoreApplication>
#include <QSettings>

#include "config.h"
#include "app_options_saver.h"

qsettings_p get_app_options_settings()
{
    return qsettings_p(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                     qApp->organizationName(), qApp->applicationName()));
}

void save_server_favs(const app_options& opts)
{
    const server_fav_list& ol = opts.servers;

    qsettings_p s = get_app_options_settings();

    s->beginWriteArray("favorites");
    int i = 0;
    for (server_fav_list::const_iterator it = ol.begin(); it != ol.end(); it++)
    {
        const server_options& so = it->second;
        s->setArrayIndex(i);
        s->setValue("name", so.name);
        s->setValue("address", so.id.address());
        s->setValue("comment", so.comment);
        s->setValue("password", so.password);
        s->setValue("rcon_password", so.rcon_password);
        s->setValue("ref_password", so.ref_password);
        i++;
    }
    s->endArray();
}

void load_server_favs(app_options& opts)
{
    server_fav_list& ol = opts.servers;

    qsettings_p s = get_app_options_settings();

    ol.clear();
    int size = s->beginReadArray("favorites");
    for (int i = 0; i < size; i++)
    {
        s->setArrayIndex(i);
        server_options so;
        so.name = s->value("name", so.name).toString();
        so.id = server_id( s->value("address", so.id.address()).toString() );
        so.comment = s->value("comment", so.comment).toString();
        so.password = s->value("password", so.password).toString();
        so.rcon_password = s->value("rcon_password", so.rcon_password).toString();
        so.ref_password = s->value("ref_password", so.ref_password).toString();
        ol[so.id] = so;
    }
    s->endArray();
}

