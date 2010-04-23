#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>

#include "main_window.h"
#include "application.h"
#include "exception.h"
#include "debughelp.h"
#include "strconvert.h"

int main(int argc, char *argv[])
{
    // this needed to link debug functions
    //    debug_help_init();

    application a(argc, argv);
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
