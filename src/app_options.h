#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>

#include "server_options.h"
#include "qstat_options.h"

class app_options
{
public:
    app_options();

    /*! start with hidden main window */
    bool start_hidden;

    /*! Use advanced command line advCmdLine() to launch UrT binary instead of binaryPath(). */
    bool use_adv_cmd_line;

    /*! Advanced command line to launch UrT binary. Use followed substitutions:
        %host% %port% %user% %password% %rcon% and so on.*/
    QString adv_cmd_line;

    /*! Path to UrT binary, used when not advanced command line. */
    QString binary_path;

    /*! Servers options list. */
    server_fav_list servers;
    
    qstat_options qstat_opts;

    /*! A path to GeoIP database file */
    QString geoip_database;

    /*! Look for system clipboard (LFC) */
    bool looking_for_clip;

    /*! LFC regexp */
    QString lfc_regexp;
    
    /*! LFC regexp match number for host */
    int lfc_host;
    
    /*! LFC regexp match number for port */
    int lfc_port;
    
    /*! LFC regexp match number for password */
    int lfc_password;
};

#endif
