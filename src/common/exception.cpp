#include <string>
#include "exception.h"

qexception::qexception( const QString & msg )
  : std::runtime_error(std::string(msg.toLocal8Bit()))
{
}

qexception::qexception( const char * msg )
  : std::runtime_error(msg)
{
}

qexception::~qexception( ) throw ()
{
}

QString qexception::message() const
{
    return QString::fromLocal8Bit(what());
}


