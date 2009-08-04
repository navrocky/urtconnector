#include <string>
#include "exception.h"

Exception::Exception( const QString & msg )
  : std::runtime_error(std::string(msg.toLocal8Bit()))
{
}

Exception::Exception( const char * msg )
  : std::runtime_error(msg)
{
}

Exception::~Exception( ) throw ()
{
}

QString Exception::message() const
{
    return QString::fromLocal8Bit(what());
}


