#ifndef _DEBUGHELP_H
#define	_DEBUGHELP_H

#include <QString>
#include <QStringRef>

const int debug_string_size = 100;

struct debug_string
{
    char str[debug_string_size];
};

void debug_help_init();

// this function needed to watch QStrings in debugger watch window
debug_string __str(const QString& s);
debug_string __str(const QStringRef& s);

#endif	/* _DEBUGHELP_H */

