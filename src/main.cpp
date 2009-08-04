#include <QMessageBox>
#include "mainwindow.h"
#include "application.h"
#include "exception.h"

/* endenis@gmail.com */

int main(int argc, char *argv[])
{
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
