#include <QCoreApplication>
#include <QSettings>

#include "config.h"
#include "app_options_saver.h"

qsettings_p get_app_options_settings(const QString& file_name)
{
    return qsettings_p(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                     qApp->applicationName(), file_name));
}

void save_app_options(qsettings_p s, app_options_p opts)
{
    s->beginGroup("app_opts");
    s->setValue("start_hidden", opts->start_hidden);
    s->setValue("center_current_row", opts->center_current_row);
    s->setValue("separate_xsession", opts->separate_x);
    s->setValue("use_adv_cmd_line", opts->use_adv_cmd_line);
    s->setValue("adv_cmd_line", opts->adv_cmd_line);
    s->setValue("binary_path", opts->binary_path);
    s->setValue("geoip_database", opts->geoip_database);
    s->setValue("keep_history", opts->keep_history);
    s->setValue("number_in_history", opts->number_in_history);
    s->endGroup();

    s->beginGroup("qstat_opts");
    s->setValue("master_server", opts->qstat_opts.master_server);
    s->setValue("qstat_path", opts->qstat_opts.qstat_path);
    s->endGroup();
    
    s->beginGroup("clipboard");
    s->setValue("looking_for", opts->looking_for_clip);
    s->setValue("regexp", opts->lfc_regexp);
    s->setValue("host", opts->lfc_host);
    s->setValue("port", opts->lfc_port);
    s->setValue("password", opts->lfc_password);
    s->endGroup();
}

void load_app_options(qsettings_p s, app_options_p opts)
{
    s->beginGroup("app_opts");
    opts->start_hidden = s->value("start_hidden", false).toBool();
    opts->center_current_row = s->value("center_current_row", true).toBool();
    opts->separate_x = s->value("separate_xsession", false).toBool();
    opts->use_adv_cmd_line = s->value("use_adv_cmd_line", opts->use_adv_cmd_line).toBool();
    opts->adv_cmd_line = s->value("adv_cmd_line", opts->adv_cmd_line).toString();
    opts->binary_path = s->value("binary_path", opts->binary_path).toString();
    opts->geoip_database = s->value( "geoip_database", opts->geoip_database ).toString();
    opts->keep_history = s->value("keep_history", opts->keep_history).toBool();
    opts->number_in_history = s->value("number_in_history", opts->number_in_history).toInt();
    s->endGroup();

    s->beginGroup("qstat_opts");
    opts->qstat_opts.master_server = s->value("master_server", opts->qstat_opts.master_server).toString();
    opts->qstat_opts.qstat_path = s->value("qstat_path", opts->qstat_opts.qstat_path).toString();
    s->endGroup();
    
    s->beginGroup("clipboard");
    opts->looking_for_clip = s->value("looking_for", opts->looking_for_clip).toBool();
    opts->lfc_regexp = s->value("regexp", opts->lfc_regexp).toString();
    opts->lfc_host = s->value("host", opts->lfc_host).toInt();
    opts->lfc_port = s->value("port", opts->lfc_port).toInt();
    opts->lfc_password = s->value("password", opts->lfc_password).toInt();
    s->endGroup();
}

void save_server_favs(qsettings_p s, app_options_p opts)
{
    const server_fav_list& ol = opts->servers;

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

void load_server_favs(qsettings_p s, app_options_p opts)
{
    server_fav_list& ol = opts->servers;

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

