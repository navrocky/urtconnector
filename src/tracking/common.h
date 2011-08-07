#ifndef TRACKING_COMMON_H
#define	TRACKING_COMMON_H

#include <QMap>
#include <QVariant>

namespace tracking
{

typedef QMap<QString, QVariant> settings_t;

template <typename T>
bool get_option(const settings_t& s, const QString& key, T& out)
{
    settings_t::const_iterator it = s.find(key);
    if (it != s.end())
    {
        out = it.value().value<T>();
        return true;
    } else
        return false;
}

}

Q_DECLARE_METATYPE(tracking::settings_t)

#endif

