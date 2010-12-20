#ifndef _QSTAT_OPTIONS_H
#define	_QSTAT_OPTIONS_H

#include <QString>

#include "settings/settings.h"

class qstat_settings: public settings_uid_provider<qstat_settings>
{
public:
    QString qstat_path() const;
    void set_qstat_path( const QString& path );
    
    QString master_server() const;
    void set_master_server( const QString& server );
};

#endif	/* _QSTATOPTIONS_H */

