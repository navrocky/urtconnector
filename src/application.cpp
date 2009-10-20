#include <QMessageBox>

#include "application.h"
#include "exception.h"

Application::Application(int &argc, char * argv[])
    : QApplication(argc, argv)
{
}

Application::~Application()
{
}

bool Application::notify(QObject * receiver, QEvent * event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch(const std::exception& e)
    {
        showError(QString::fromLocal8Bit(e.what()));
    }
    catch(...)
    {
        showError(QObject::tr("Unknown error"));
    }
    return false;
}

void Application::showError(const QString & msg)
{
    QMessageBox::critical(NULL, QObject::tr("Error"), msg);
}

