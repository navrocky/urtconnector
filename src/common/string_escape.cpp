#include <stdexcept>
#include "string_escape.h"

QString string_escape(const QString& src, const QString& chars_to_escape, const QChar& escape_char)
{
    QString res;
    for (int i = 0; i < src.length(); i++)
    {
        QChar ch = src[i];
        if (chars_to_escape.indexOf(ch) >= 0 || ch == escape_char)
        {
            res += QString("\\%1").arg(ch.unicode(), 4, 16, QLatin1Char('0'));
        }
        else
            res += ch;
    }
    return res;
}

QString string_unescape(const QString& src, const QChar& escape_char)
{
    QString result = src;
    int pos = 0;

    while (true)
    {
        pos = result.indexOf(escape_char, pos);
        if (pos < 0)
            break;
        bool ok = true;
        ushort code = result.mid(pos + 1, 4).toUShort(&ok, 16);
        if (!ok)
            throw std::runtime_error("Invalid escaped character hex code");
        QChar ch(code);
        result.replace(pos, 5, ch);
        pos++;
    }

    return result;
}
