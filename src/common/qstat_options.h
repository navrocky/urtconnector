#ifndef _QSTAT_OPTIONS_H
#define	_QSTAT_OPTIONS_H

#include <QString>
#include <settings/settings_generator.h>

#define QSTAT_OPTIONS \
    (( master_server, QString, QString("master.urbanterror.info") )) \
    (( max_sim_queries, int, 0 )) \
    (( timeout, int, 2000 )) \
    (( series_timeout, int, 100 )) \
    (( retry_number, int, 2 ))

SETTINGS_GENERATE_CLASS(qstat_options, QSTAT_OPTIONS)

#endif

