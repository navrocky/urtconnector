#include <cstring>
#include <string>
#include <QtCore/qhash.h>
#include "debughelp.h"

debug_string __str_int(char* s)
{
    debug_string dst;
    strncpy(&(dst.str[0]), s, debug_string_size);
    return dst;
}

debug_string __str(const QString& s)
{
    return __str_int(s.toLocal8Bit().data());
}

debug_string __str(const QStringRef& s)
{
    return __str_int(s.toString().toLocal8Bit().data());
}


void debug_help_init()
{
    __str("a");
    QString s;
    __str(QStringRef(&s));
}