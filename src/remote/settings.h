
#ifndef URT_REMOTE_SETTINGS
#define URT_REMOTE_SETTINGS

#include "QStringList"

#include <settings/settings_generator.h>


#define MANAGER_OPTIONS \
    (( storages, QStringList, QStringList() ))

SETTINGS_GENERATE_CLASS(manager_options, MANAGER_OPTIONS);


// #undef MANAGER_OPTIONS;

#endif

