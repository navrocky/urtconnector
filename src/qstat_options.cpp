
#include <QSettings>

#include "qstat_options.h"

#if defined(Q_OS_UNIX)
    const QString default_qstat_c = "/usr/bin/qstat";
#elif defined(Q_OS_WIN)
    const QString default_qstat_c = "qstat.exe";
#elif defined(Q_OS_MAC)
    const QString default_qstat_c = "/usr/bin/qstat";
#endif

QString qstat_settings::qstat_path() const
{
    return part()->value("qstat_path", default_qstat_c).toString();
}

void qstat_settings::set_qstat_path(const QString& path)
{
    part()->setValue("qstat_path", path);
}


QString qstat_settings::master_server() const
{
    return part()->value("master_server", "master.urbanterror.net").toString();
}

void qstat_settings::set_master_server(const QString& server)
{
    part()->setValue("master_server", server);
}


