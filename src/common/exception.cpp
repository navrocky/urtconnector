#include <string>

#include "qt_syslog.h"
#include "exception.h"

SYSLOG_MODULE("exception")

qexception::qexception( const QString & msg )
  : std::runtime_error(std::string(msg.toLocal8Bit()))
{
    LOG_ERR << msg;
}

qexception::qexception( const char * msg )
  : std::runtime_error(msg)
{
    LOG_ERR << msg;
}

qexception::~qexception( ) throw ()
{
}

QString qexception::message() const
{
    return QString::fromLocal8Bit(what());
}


