#include <cassert>

#include <QObject>
#include <QChar>

#include <common/exception.h>

#include "simple_database.h"

////////////////////////////////////////////////////////////////////////////////
// database

database* database::instance_ = NULL;

database::database()
{
    assert(!instance_);
    instance_ = this;
}

database::~database()
{
    instance_ = NULL;
}

QString database::qqencode_string(const QString& str)
{
    QString res;
    res.reserve(str.length());

    static const QString c_hex("#%1");
    static const QChar c_zero('0');
    foreach (const QChar& ch, str)
    {
        if (ch.isLetterOrNumber())
            res += ch;
        else
            res += c_hex.arg(ch.unicode(), 4, 16, c_zero);
    }
    return res;
}

QString database::qqdecode_string(const QString& str)
{
    QString res;
    int size = str.length();
    res.reserve(size);

    static const QChar c_grid('#');

    for (int i = 0; i < size; i++)
    {
        const QChar& ch = str[i];
        if (ch == c_grid)
        {
            bool ok = false;
            ushort val = str.mid(i+1, 4).toUShort(&ok, 16);
            if (!ok)
                throw qexception(QObject::tr("Incorrect hex sequence"));
            res += QChar(val);
            i += 4;
        } else
            res += ch;
    }
    return res;
}
