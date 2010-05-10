
#include "config.h"

#include "app_options.h"


app_options::app_options()
 : main( settings::User )
 , launch( main.entry("launch") )
 , qstat( main.entry("qstat") )
 , srv_list( main.entry("srv_list", settings::Split) )
 , state( main.entry("state", settings::Split) )
{
    //setting up default values

    main.set_value( "start_hidden", main.value( "start_hidden", false ) );
    main.set_value( "looking_for_clip", main.value( "looking_for_clip", false ) );
    main.set_value( "geoip_database", main.value<QString>("geoip_database", QString(URT_DATADIR) + "GeoIP.dat" ) );

#if defined(Q_OS_UNIX)
    qstat.set_value( "path", qstat.value( "path", "/usr/bin/qstat" )  );
#elif defined(Q_OS_WIN)
    qstat.set_value( "path", qstat.value( "path", QString("qstat.exe") )  );
#elif defined(Q_OS_MAC)
    qstat.set_value( "path", qstat.value( "path", "/usr/bin/qstat" )  );
#endif

    qstat.set_value( "master_server", qstat.value( "master_server", QString("master.urbanterror.net") ) );
    
    launch.set_value( "use_adv_cmd_line", launch.value( "use_adv_cmd_line", false ) );

    sync();
}

void app_options::sync()
{
    main.sync();
    launch.sync();
    qstat.sync();
    srv_list.sync();
    state.sync();
}

