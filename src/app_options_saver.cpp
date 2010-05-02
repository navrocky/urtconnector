#include <QCoreApplication>
#include <QSettings>

#include "config.h"
#include "app_options_saver.h"

qsettings_p get_app_options_settings()
{
    return qsettings_p(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                     qApp->organizationName(), qApp->applicationName()));
}

void save_app_options(qsettings_p s, const app_options& opts)
{
    s->beginGroup("app_opts");
    s->setValue("start_hidden", opts.start_hidden);
    s->setValue("looking_clip", opts.looking_for_clip);
    s->setValue("use_adv_cmd_line", opts.use_adv_cmd_line);
    s->setValue("adv_cmd_line", opts.adv_cmd_line);
    s->setValue("binary_path", opts.binary_path);
    s->setValue("geoip_database", opts.geoip_database);
    s->endGroup();

    s->beginGroup("qstat_opts");
    s->setValue("master_server", opts.qstat_opts.master_server);
    s->setValue("qstat_path", opts.qstat_opts.qstat_path);
    s->endGroup();
}

void load_app_options(qsettings_p s, app_options& opts)
{
    s->beginGroup("app_opts");
    opts.start_hidden = s->value("start_hidden", false).toBool();
    opts.looking_for_clip = s->value("looking_clip", true).toBool();
    opts.use_adv_cmd_line = s->value("use_adv_cmd_line", opts.use_adv_cmd_line).toBool();
    opts.adv_cmd_line = s->value("adv_cmd_line", opts.adv_cmd_line).toString();
    opts.binary_path = s->value("binary_path", opts.binary_path).toString();
    opts.geoip_database = s->value( "geoip_database", opts.geoip_database ).toString();
    s->endGroup();

    s->beginGroup("qstat_opts");
    opts.qstat_opts.master_server = s->value("master_server", opts.qstat_opts.master_server).toString();
    opts.qstat_opts.qstat_path = s->value("qstat_path", opts.qstat_opts.qstat_path).toString();
    s->endGroup();
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

