#include <iostream>
#include <boost/program_options.hpp>

#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTemporaryFile>
#include <QDesktopWidget>

#include <cl/syslog/output_stream.h>
#include <common/qt_syslog.h>
#include <common/state_settings.h>
#include <common/exception.h>
#include <launcher/launcher.h>
#include <anticheat/settings.h>
#include <anticheat/tools.h>
#include <rcon/rcon_settings.h>
#include <settings/settings.h>

#include "main_window.h"
#include "application.h"
#include "debug_help.h"
#include "str_convert.h"
#include "tools.h"
#include "config.h"
#include "pointers.h"

using namespace cl::syslog;
using namespace std;
namespace po = boost::program_options;

SYSLOG_MODULE(main)

void show_help(const po::options_description& desc)
{
    cout << "\nUrTConnector " << URT_VERSION << "\n\n";
    cout << desc << "\n";
}

int main(int argc, char *argv[])
{
    bool gui_enabled = false;
    output_p cerr_out(new output_stream(std::cerr));
    logman().output_add(cerr_out);
    QString error_str;
    try
    {
        // parsing program options
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "Produce help message")
                ("debug", "Produce debug messages")
                ("launch", "Quick launch game")
                ;

        po::variables_map vm;
        try
        {
            po::store(po::parse_command_line(argc, argv, desc), vm);
        }
        catch(const std::exception& e)
        {
            cerr << "\nError: " << e.what() << endl;
            show_help(desc);
            return 1;
        }
        po::notify(vm);

        if (vm.count("help"))
        {
            show_help(desc);
            return 0;
        }

        bool debug_enabled = vm.count("debug");
        if (debug_enabled)
            logman().level_set(debug);
        else
            logman().level_set(info);

        LOG_DEBUG << "Syslog started";

        //Initializing main settings
        base_settings set;
        //Registering state_settings in separate file
        set.register_file( state_settings::uid(), "state.ini" );
        set.register_file( server_list_widget_settings::uid(), "options.ini" );
        set.register_group( rcon_settings::uid(), "rcon", "options.ini" );
        set.register_group( anticheat::settings::uid(), "anticheat", "options.ini" );

        if (vm.count("launch"))
        {
            LOG_DEBUG << "Quick launch";
            QApplication a(argc, argv, false);

            app_options_p opts;
//            anticheat::manager anticheat;
            launcher l(opts);
            l.launch();
            a.exec();
            return 0;
        }

        application a(argc, argv);
        gui_enabled = true;

#ifdef USE_SINGLE_APP
        if ( a.isRunning() )
        {
            LOG_DEBUG << "UrTConnector already started. Activating and exit.";

            a.sendMessage( QString() );
            return 0;
        }
#endif
        application::setQuitOnLastWindowClosed(false);

        a.setOrganizationName("urtcommunity");
        a.setApplicationName("urtconnector");

        // loading translations
        QTranslator qt_trans;
        QString trans_name = "qt_" + QLocale::system().name();
        bool loaded = qt_trans.load(trans_name,
                                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        if (loaded)
            LOG_DEBUG << "Translation \"%1\" loaded", trans_name;
        else
            LOG_DEBUG << "Failed to load translation \"%1\"", trans_name;

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
            LOG_DEBUG << "Translation \"%1\" loaded", trans_name;
        else
            LOG_DEBUG << "Failed to load translation \"%1\"", trans_name;
        
        a.installTranslator(&urt_tr);

        main_window w;

#ifdef USE_SINGLE_APP
        //set a widget that should raise when new instance trying to start
        a.setActivationWindow(&w);
#endif
        int res = a.exec();
        
        LOG_DEBUG << "Application finished";
        
        return res;
    }
    catch (const std::exception& e)
    {
        error_str = to_qstr(e.what());
    }
    catch (...)
    {
        error_str = QObject::tr("Unknown error");
    }

    LOG_ERR << "Exception: %1", error_str;
    application* app = qobject_cast<application*>(qApp);
    if (gui_enabled && app)
        app->show_error(error_str);

    return 1;
}
