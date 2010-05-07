#include <QMessageBox>

#include "application.h"
#include "exception.h"
#include "str_convert.h"

application::application(int &argc, char * argv[])
    : QtSingleApplication(argc, argv)
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



