#ifndef STATE_SETTINGS_H
#define STATE_SETTINGS_H

#include <QByteArray>

#include <settings/settings.h>

/// Application state settings
class state_settings: public settings_uid_provider<state_settings>
{
public:
    QByteArray geometry() const;
    void set_geometry(const QByteArray& g);
};

#endif
