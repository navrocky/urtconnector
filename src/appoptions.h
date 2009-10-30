#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>

#include "serveroptions.h"
#include "qstatoptions.h"

class AppOptions
{
public:
    AppOptions();

    /*! Use advanced command line advCmdLine() to launch UrT binary instead of binaryPath(). */
    bool useAdvCmdLine;

    /*! Advanced command line to launch UrT binary. Use followed substitutions:
        %host% %port% %user% %password% %rcon% and so on.*/
    QString advCmdLine;

    /*! Path to UrT binary, used when not advanced command line. */
    QString binaryPath;

    /*! Servers options list. */
    ServerOptionsList servers;
    
    QStatOptions qstatOpts;
};

typedef AppOptions* AppOptionsPtr;

#endif
