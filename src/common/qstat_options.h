#ifndef _QSTAT_OPTIONS_H
#define	_QSTAT_OPTIONS_H

#include <QString>
#include <settings/settings_generator.h>

#if defined(Q_OS_UNIX)
#define QSTAT_DEF_PATH "/usr/bin/qstat"
#elif defined(Q_OS_WIN)
#define QSTAT_DEF_PATH "qstat.exe"
#elif defined(Q_OS_MAC)
#define QSTAT_DEF_PATH "/usr/bin/qstat"
#endif

#define QSTAT_OPTIONS \
    (( qstat_path, QString, QString(QSTAT_DEF_PATH) )) \
    (( master_server, QString, QString("master.urbanterror.net") )) \
    (( max_sim_queries, int, 200 )) \
    (( retry_number, int, 15 )) \
    (( retry_interval, double, 0.5 )) \
    (( retry_master_interval, double, 0.1 ))

SETTINGS_GENERATE_CLASS(qstat_options, QSTAT_OPTIONS)

#endif

