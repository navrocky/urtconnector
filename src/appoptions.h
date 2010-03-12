#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>
#include <QSharedPointer>

#include "serveroptions.h"
#include "qstatoptions.h"

class app_options
{
public:
    app_options();

    /*! Use advanced command line advCmdLine() to launch UrT binary instead of binaryPath(). */
    bool use_adv_cmd_line;

    /*! Advanced command line to launch UrT binary. Use followed substitutions:
        %host% %port% %user% %password% %rcon% and so on.*/
    QString adv_cmd_line;

    /*! Path to UrT binary, used when not advanced command line. */
    QString binary_path;

    /*! Servers options list. */
    ServerOptionsList servers;
    
    qstat_options qstat_opts;
};

typedef QSharedPointer<app_options> app_options_ptr;

#endif
