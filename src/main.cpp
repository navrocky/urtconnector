#include <QMessageBox>
#include "mainwindow.h"
#include "application.h"
#include "exception.h"
#include "debughelp.h"
#include "strconvert.h"

int main(int argc, char *argv[])
{
    // this needed to link debug functions
    debug_help_init();

    application a(argc, argv);
    try
    {
        a.setOrganizationName("urtcommunity");
        a.setApplicationName("urtconnector");

        main_window w;
//        w.show();
        return a.exec();
    }
    catch(const std::exception& e)
    {
        a.show_error(to_qstr(e.what()));
    }
    catch(...)
    {
        a.show_error(QObject::tr("Unknown error"));
    }
    return 1;
}
