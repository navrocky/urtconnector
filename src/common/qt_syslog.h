#ifndef _QT_SYSLOG_H
#define	_QT_SYSLOG_H

#include <QString>
#include <cl/syslog/syslog.h>

namespace cl
{
namespace syslog
{

template<>
autolog operator<< <QString>(const module& m, const QString& value);

template <>
const autolog& operator<<(const autolog& al, const QString& value);

template <>
const autolog& operator,(const autolog& al, const QString& value);

}
}

#endif	/* _QT_SYSLOG_H */

