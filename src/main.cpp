#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>

#include <iostream>
#include <cl/syslog/syslog.h>
#include <cl/syslog/output_stream.h>

#include "main_window.h"
#include "application.h"
#include "exception.h"
#include "debug_help.h"
#include "str_convert.h"

using namespace cl::syslog;

int main(int argc, char *argv[])
{
    output_p cerr_out(new output_stream(std::cerr));
//     logman().level_set(harddebug);
    logman().output_add(cerr_out);
    
    // this needed to link debug functions
    //    debug_help_init();
    application a(argc, argv);
    if ( a.isRunning() )
    {
        a.sendMessage( QString() );
        return 0;
    }

    try
    {
        a.setOrganizationName("urtcommunity");
        a.setApplicationName("urtconnector");

        // loading translations
        QTranslator qt_trans;
        qt_trans.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        a.installTranslator(&qt_trans);

        QTranslator urt_tr;

#if defined(Q_OS_UNIX)
        urt_tr.load("urtconnector_" + QLocale::system().name(), "/usr/share/urtconnector/translations");
#elif defined(Q_OS_WIN)
        urt_tr.load("urtconnector_" + QLocale::system().name());
#elif defined(Q_OS_MAC)
        // FIXME i don't know how do this on mac
        urt_tr.load("urtconnector_" + QLocale::system().name());
#endif
        a.installTranslator(&urt_tr);

        main_window w;
        //set a widget that should raise when new instance trying to start
        a.setActivationWindow(&w);
        return a.exec();
    }
    catch (const std::exception& e)
    {
        a.show_error(to_qstr(e.what()));
    }
    catch (...)
    {
        a.show_error(QObject::tr("Unknown error"));
    }
    return 1;
}
