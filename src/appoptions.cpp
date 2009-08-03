#include "appoptions.h"

AppOptions::AppOptions()
 : Options(),
   useAdvCmdLine_(false),
   advCmdLine_(),
   binaryPath_()
{
}

AppOptions::~AppOptions()
{
}

void AppOptions::setUseAdvCmdLine(bool value)
{
    useAdvCmdLine_ = value;
}

void AppOptions::setAdvCmdLine(const QString & value)
{
    advCmdLine_ = value;
}

void AppOptions::setBinaryPath( const QString& value )
{
    binaryPath_ = value;
}

void AppOptions::operator =(const AppOptions & src)
{
    setAdvCmdLine( src.advCmdLine() );
    setBinaryPath( src.binaryPath() );
    setUseAdvCmdLine( src.useAdvCmdLine() );
}

