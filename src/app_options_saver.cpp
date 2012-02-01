#include <QCoreApplication>
#include <QSettings>

#include "config.h"
#include "app_options_saver.h"

qsettings_p get_app_options_settings(const QString& file_name)
{
    return qsettings_p(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                     qApp->applicationName(), file_name));
}
/*
void save_app_options(qsettings_p s, app_options_p opts)
{
    s->beginGroup("app_opts");
//     s->setValue("start_hidden", opts->start_hidden);
//     s->setValue("center_current_row", opts->center_current_row);
//     s->setValue("separate_xsession", opts->separate_x);
//     s->setValue("use_adv_cmd_line", opts->use_adv_cmd_line);
//     s->setValue("adv_cmd_line", opts->adv_cmd_line);
//     s->setValue("binary_path", opts->binary_path);
//     s->setValue("geoip_database", opts->geoip_database);
//     s->setValue("keep_history", opts->keep_history);
//     s->setValue("number_in_history", opts->number_in_history);
//     s->endGroup();
// 
//     s->beginGroup("qstat_opts");
//     s->setValue("master_server", opts->qstat_opts.master_server);
//     s->setValue("qstat_path", opts->qstat_opts.qstat_path);
//     s->endGroup();
//     
//     s->beginGroup("clipboard");
//     s->setValue("looking_for", opts->looking_for_clip);
//     s->setValue("regexp", opts->lfc_regexp);
//     s->setValue("host", opts->lfc_host);
//     s->setValue("port", opts->lfc_port);
//     s->setValue("password", opts->lfc_password);
    s->endGroup();
}

void load_app_options(qsettings_p s, app_options_p opts)
{
    s->beginGroup("app_opts");
//     opts->start_hidden = s->value("start_hidden", false).toBool();
//     opts->center_current_row = s->value("center_current_row", true).toBool();
//     opts->separate_x = s->value("separate_xsession", false).toBool();
//     opts->use_adv_cmd_line = s->value("use_adv_cmd_line", opts->use_adv_cmd_line).toBool();
//     opts->adv_cmd_line = s->value("adv_cmd_line", opts->adv_cmd_line).toString();
//     opts->binary_path = s->value("binary_path", opts->binary_path).toString();
//     opts->geoip_database = s->value( "geoip_database", opts->geoip_database ).toString();
//     opts->keep_history = s->value("keep_history", opts->keep_history).toBool();
//     opts->number_in_history = s->value("number_in_history", opts->number_in_history).toUInt();
    s->endGroup();

    s->beginGroup("qstat_opts");
//     opts->qstat_opts.master_server = s->value("master_server", opts->qstat_opts.master_server).toString();
//     opts->qstat_opts.qstat_path = s->value("qstat_path", opts->qstat_opts.qstat_path).toString();
    s->endGroup();
    
    s->beginGroup("clipboard");
//     opts->looking_for_clip = s->value("looking_for", opts->looking_for_clip).toBool();
//     opts->lfc_regexp = s->value("regexp", opts->lfc_regexp).toString();
//     opts->lfc_host = s->value("host", opts->lfc_host).toInt();
//     opts->lfc_port = s->value("port", opts->lfc_port).toInt();
//     opts->lfc_password = s->value("password", opts->lfc_password).toInt();
    s->endGroup();
}*/

void save_server_bookmarks(qsettings_p s, server_bookmark_list* bml)
{
    s->beginWriteArray("favorites");
    int i = 0;
    foreach (const server_bookmark& bm, bml->list())
    {
        s->setArrayIndex(i);
        s->setValue("name", bm.name());
        s->setValue("address", bm.id().address());
        s->setValue("comment", bm.comment());
        s->setValue("password", bm.password());
        s->setValue("rcon_password", bm.rcon_password());
        s->setValue("ref_password", bm.ref_password());
        s->setValue("sync_stamp", bm.sync_stamp());
        i++;
    }
    s->endArray();
}

void load_server_bookmarks(qsettings_p s, server_bookmark_list* bml)
{
    bml->clear();
    int size = s->beginReadArray("favorites");
    for (int i = 0; i < size; i++)
    {
        s->setArrayIndex(i);
        server_bookmark bm(server_id(s->value("address").toString()),
                           s->value("name").toString(),
                           s->value("comment").toString(),
                           s->value("password").toString(),
                           s->value("rcon_password").toString(),
                           s->value("ref_password").toString(),
                           s->value("sync_stamp").toDateTime() );
        bml->add(bm);
    }
    s->endArray();
}
