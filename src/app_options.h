#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>

#include "server_options.h"

#include "settings/settings.h"

class app_options
{
public:
    app_options();

    ///main configuration object
    settings    main;
    ///prefferences for launching UT
    settings    launch;
    ///qstat prefferences
    settings    qstat;

    
    settings    srv_list;

    ///states of the widgets are stored here
    settings    state;

    void sync();
    
    // Servers options list.
    server_fav_list servers;
};

#endif
