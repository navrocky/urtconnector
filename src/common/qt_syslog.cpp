#include "qt_syslog.h"

namespace cl
{
namespace syslog
{

template<>
autolog operator<< <QString>(const module& m, const QString& value)
{
    return autolog(message(default_level, m.module_name(), value.toLocal8Bit().data()));
}

template <>
const autolog& operator<<(const autolog& al, const QString& value)
{
    al.append_to_msg(value.toLocal8Bit().data());
    return al;
}

template <>
const autolog& operator,(const autolog& al, const QString& value)
{
    al.insert_to_msg(value.toLocal8Bit().data());
    return al;
}

}
}