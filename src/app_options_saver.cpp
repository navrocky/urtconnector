#include <QCoreApplication>
#include <QSettings>

#include "app_options_saver.h"

void save_app_options(const app_options& opts)
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName());
    s.beginGroup("app_opts");
    s.setValue("use_adv_cmd_line", opts.use_adv_cmd_line);
    s.setValue("adv_cmd_line", opts.adv_cmd_line);
    s.setValue("binary_path", opts.binary_path);
    s.endGroup();

    s.beginGroup("qstat_opts");
    s.setValue("master_server", opts.qstat_opts.master_server);
    s.setValue("qstat_path", opts.qstat_opts.qstat_path);
    s.endGroup();
}

void load_app_options(app_options& opts)
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName());

    s.beginGroup("app_opts");
    opts.use_adv_cmd_line = s.value("use_adv_cmd_line", opts.use_adv_cmd_line).toBool();
    opts.adv_cmd_line = s.value("adv_cmd_line", opts.adv_cmd_line).toString();
    opts.binary_path = s.value("binary_path", opts.binary_path).toString();
    s.endGroup();

    s.beginGroup("qstat_opts");
    opts.qstat_opts.master_server = s.value("master_server", opts.qstat_opts.master_server).toString();
    opts.qstat_opts.qstat_path = s.value("qstat_path", opts.qstat_opts.qstat_path).toString();
    s.endGroup();
}
