#include <QMessageBox>

#include <common/exception.h>
#include <common/str_convert.h>
#include "application.h"

application::application(int &argc, char * argv[])
#ifdef USE_SINGLE_APP
: QtSingleApplication(argc, argv)
#else
: QApplication(argc, argv)
#endif
{
}

application::~application()
{
}

bool application::notify(QObject * receiver, QEvent * event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch(const std::exception& e)
    {
        show_error(to_qstr(e.what()));
    }
    catch(...)
    {
        show_error(QObject::tr("Unknown error"));
    }
    return false;
}

void application::show_error(const QString & msg)
{
    QMessageBox::critical(NULL, QObject::tr("Error"), msg);
}



