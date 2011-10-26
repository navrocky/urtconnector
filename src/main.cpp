#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QLocale>

#include <cl/syslog/output_stream.h>
#include <cl/syslog/output_file.h>
#include <common/qt_syslog.h>
#include <common/state_settings.h>
#include <common/exception.h>
#include <common/str_convert.h>
#include <common/tools.h>
#include <common/qstat_options.h>
#include <launcher/launcher.h>
#include <anticheat/settings.h>
#include <anticheat/tools.h>
#include <rcon/rcon_settings.h>
#include <settings/settings.h>
#include <database/simple_database.h>
#include <database/sqlite_database.h>
#include "server_list_saver.h"

#include "main_window.h"
#include "application.h"
#include "debug_help.h"
#include "config.h"
#include "pointers.h"
#include "app_options_saver.h"

using namespace cl::syslog;
using namespace std;
namespace po = boost::program_options;

SYSLOG_MODULE(main)

void show_help(const po::options_description& desc)
{
    cout << "\nUrTConnector " << URT_VERSION << "\n\n";
    cout << desc << "\n";
}

void init_application(QApplication* a)
{
    a->setOrganizationName("urtcommunity");
    a->setApplicationName("urtconnector");

    //Initializing main settings
    base_settings set;
    set.register_group( app_settings::uid(),   "app_opts",   "options.ini" );    
    
    // loading translations
    static boost::shared_ptr<QTranslator> qt_trans = system_translator( app_settings().country_name() );
    
    if ( !qt_trans->isEmpty() )
    {
        LOG_DEBUG << "Translation \"%1\" loaded", app_settings().country_name();
    }
    else
    {
        LOG_DEBUG << "Failed to load system translation \"%1\". Trying \"%2\" instead...", app_settings().country_name(), QLocale::system().name();
        
        qt_trans = system_translator( QLocale::system().name() );
        if( !qt_trans->isEmpty() )
            LOG_DEBUG << "Translation \"%1\" loaded", QLocale::system().name();
        else
            LOG_DEBUG << "Failed to load system translation \"%1\"", QLocale::system().name();
    }

    a->installTranslator(qt_trans.get());
    
    static boost::shared_ptr<QTranslator> urt_tr = local_translator( app_settings().country_name() );

    if ( !urt_tr->isEmpty()  )
    {
        LOG_DEBUG << "Translation \"%1\" loaded", app_settings().country_name();
    }
    else
    {
        LOG_DEBUG << "Failed to load local translation \"%1\"", app_settings().country_name();
    }

    a->installTranslator(urt_tr.get());

    QTextCodec::setCodecForTr( QTextCodec::codecForName("utf8") );

    set.register_group( clip_settings::uid(),  "clipboard",  "options.ini" );
    set.register_group( qstat_options::uid(), "qstat_opts", "options.ini" );
    
    //Registering state_settings in separate file
    set.register_file( state_settings::uid(), "state.ini" );
    set.register_group( rcon_settings::uid(), "rcon", "options.ini" );
    set.register_group( anticheat::settings::uid(), "anticheat", "options.ini" );
    
    set.register_file( "history", "history.ini" );

    //Initializing resource resolution
    // to use icons from resources you must use "icons:<name>" syntax
    // other resource syntax unchaged
    
    // detect christmas and using icons from another iconset
    if( app_settings().christmas_mode() )
    {
        QDir::addSearchPath("icons", QString(":icons/icons/christmas"));
        QDir::addSearchPath("images", QString(":images/icons/christmas"));
    }
    
    QDir::addSearchPath("icons", QString(":icons/icons/"));
    QDir::addSearchPath("images", QString(":images/icons/"));
    
    a->translate("language", "Russian");
    a->translate("language", "English");
    a->translate("language", "kitaiskij");
}

const string login_uri = "https://www.google.com/accounts/ClientLogin";

const string kDocListScope = "http://docs.google.com/feeds";
const string kDocListFeed = "/documents/private/full";

#include "gdoc.h"
#include "remote/backends/gdocs/gdocs.h"

int main(int argc, char *argv[])
{
    QApplication ap(argc, argv);
    logman().level_set(harddebug);
    
    
    output_p cerr_out;
    cerr_out.reset(new output_stream(std::cerr));
    logman().output_add(cerr_out);
    
    std::cerr<<"-1"<<std::endl;
    gdocs d("kinnalru@gmail.com", "malder11", "myapp");

    remote::action* act = d.put(remote::group("secondupload"));
    act->start();
    
    std::cerr<<"-0"<<std::endl;
    
    return ap.exec();
    
    bool gui_enabled = false;
    QString error_str;
    try
    {
        // parsing program options
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "Produce help message")
                ("debug,D", "Produce debug messages")
                ("hard", "Produce hard debug messages")
                ("pipe-log", "Redirect all logging to stderr with a special marks")
                ("anticheat,A", "Activate anticheat")
                ("launch,L", "Launch a game")
#if defined(Q_OS_UNIX)
                ("separate-x,X", "Launch game in separate X")
#endif
                ("player", po::value<string>(), "Player name")
                ("addr", po::value<string>(), "Server address")
                ("pass", po::value<string>(), "Password")
                ("rcon", po::value<string>(), "RCON password")
                ("referee", po::value<string>(), "Referee password")
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
        
        if( vm.count("hard") )
            logman().level_set(harddebug);

        output_p cerr_out, file_out;
        if (vm.count("pipe-log"))
        {
            // TODO Special log output to forward all messages to main programm
            cerr_out.reset(new output_stream(std::cerr));
            logman().output_add(cerr_out);
        } else
        {
            cerr_out.reset(new output_stream(std::cerr));
            file_out.reset(new output_file(to_str(QDesktopServices::storageLocation(
                    QDesktopServices::HomeLocation)) + "/urtconnector.log"));
            logman().output_add(cerr_out);
            logman().output_add(file_out);
        }

        LOG_DEBUG << "Syslog started";

        if (vm.count("launch"))
        {
            LOG_DEBUG << "Quick launch";
            QApplication a(argc, argv, true);
            init_application(&a);

            QString name = QObject::tr("Unnamed");
            if (vm.count("player"))
                name = to_qstr(vm["player"].as<string>());

            anticheat::anticheat* ac = vm.count("anticheat") ? anticheat::create_anticheat(name, &a) : NULL;

            launcher l;
            l.set_detach(false);
            if (vm.count("addr"))
                l.set_server_id(server_id(to_qstr(vm["addr"].as<string>())));
            if (vm.count("player"))
                l.set_user_name(to_qstr(vm["player"].as<string>()));
            if (vm.count("pass"))
                l.set_password(to_qstr(vm["pass"].as<string>()));
            if (vm.count("rcon"))
                l.set_rcon(to_qstr(vm["rcon"].as<string>()));
            if (vm.count("referee"))
                l.set_referee(to_qstr(vm["referee"].as<string>()));

            if (ac)
            {
                QObject::connect(&l, SIGNAL(started()), ac, SLOT(start()));
                QObject::connect(&l, SIGNAL(stopped()), ac, SLOT(stop()));
            }

            l.launch(l.launch_string(vm.count("separate-x")));
            QObject::connect(&l, SIGNAL(stopped()), &a, SLOT(quit()));
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
        init_application(&a);

        // initialize database
        qsettings_p s = get_app_options_settings("test");
        QDir().mkpath( QFileInfo(s->fileName()).absoluteDir().absolutePath() );
        QString fn = QFileInfo(s->fileName()).absoluteDir().absolutePath() + "/urtconnector.sqlite";
        s.reset();
        sqlite_database db(fn);
        init_database();

        // apply style sheets
        try
        {
            app_settings as;
            load_app_style_sheet(as.style_sheet_file());
        }
        catch(...)
        {
        }

        main_window w;

        // detect christmas and activate this mode if any
//        int month = QDate::currentDate().month();
//        w.set_christmas_mode(month == 1 || month == 12);

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
