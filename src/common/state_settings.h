#ifndef STATE_SETTINGS_H
#define STATE_SETTINGS_H

#include <QByteArray>
#include <settings/settings_generator.h>

/// Application state settings
#define STATE_SETTINGS \
    (( geometry, QByteArray, QByteArray() ))
    
SETTINGS_GENERATE_CLASS(state_settings, STATE_SETTINGS)

#endif
