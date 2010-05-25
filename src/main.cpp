#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QString>

#include <iostream>
#include <cl/syslog/syslog.h>
#include <cl/syslog/output_stream.h>

#include "main_window.h"
#include "application.h"
#include "exception.h"
#include "debug_help.h"
#include "str_convert.h"

using namespace cl::syslog;

SYSLOG_MODULE("main");

int main(int argc, char *argv[])
{
    output_p cerr_out(new output_stream(std::cerr));
    logman().level_set(harddebug);
    logman().output_add(cerr_out);
    
    LOG_DEBUG << "Syslog started";
    
    // this needed to link debug functions
    //    debug_help_init();
    application a(argc, argv);
    if ( a.isRunning() )
    {
        a.sendMessage( QString() );
        return 0;
    }
    application::setQuitOnLastWindowClosed(false);

    try
    {
        a.setOrganizationName("urtcommunity");
        a.setApplicationName("urtconnector");

        // loading translations
        QTranslator qt_trans;
        QString trans_name = "qt_" + QLocale::system().name();
        bool loaded = qt_trans.load(trans_name,
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        if (loaded)
            LOG_DEBUG << "Translation \"%1\" loaded", trans_name.toStdString();
        else
            LOG_DEBUG << "Failed to load translation \"%1\"", trans_name.toStdString();
            
        a.installTranslator(&qt_trans);

        QTranslator urt_tr;

        trans_name = "urtconnector_" + QLocale::system().name();
#if defined(Q_OS_UNIX)
        loaded = urt_tr.load(trans_name, "/usr/share/urtconnector/translations");
#elif defined(Q_OS_WIN)
        loaded = urt_tr.load(trans_name);
#elif defined(Q_OS_MAC)
        // FIXME i don't know how do this on mac
        loaded = urt_tr.load(trans_name);
#endif
        if (loaded)
            LOG_DEBUG << "Translation \"%1\" loaded", trans_name.toStdString();
        else
            LOG_DEBUG << "Failed to load translation \"%1\"", trans_name.toStdString();
        
        a.installTranslator(&urt_tr);

        main_window w;
        //set a widget that should raise when new instance trying to start
        a.setActivationWindow(&w);
        int res = a.exec();
        
        LOG_DEBUG << "Application finished";
        
        return res;
    }
    catch (const std::exception& e)
    {
        LOG_ERR << "Exception catched: " << e.what();
        a.show_error(to_qstr(e.what()));
    }
    catch (...)
    {
        LOG_ERR << "Unknown exception catched";
        a.show_error(QObject::tr("Unknown error"));
    }
    return 1;
}
