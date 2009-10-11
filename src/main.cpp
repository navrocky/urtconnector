#include <QMessageBox>
#include "mainwindow.h"
#include "application.h"
#include "exception.h"
#include "debughelp.h"

/* endenis@gmail.com */

int main(int argc, char *argv[])
{
    // this needed to link debug functions
    debug_help_init();

    Application a(argc, argv);
    try
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    catch(const std::exception& e)
    {
        a.showError(QString::fromLocal8Bit(e.what()));
    }
    catch(...)
    {
        a.showError(QObject::tr("Unknown error"));
    }
    return 1;
}
