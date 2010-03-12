#pragma once

#include <string>
#include <QString>

/*! Convert QString to std::string */
inline std::string to_str(const QString& s)
{
    return s.toLocal8Bit().data();
}

/*! Convert std::string to QString */
inline QString to_qstr(const std::string& s)
{
    return QString::fromLocal8Bit(s.c_str());
}

/*! Convert const char* to QString */
inline QString to_qstr(const char* s)
{
    return QString::fromLocal8Bit(s);
}
