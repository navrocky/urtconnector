#ifndef _STRING_ESCAPE_H
#define	_STRING_ESCAPE_H

#include <QString>

// replace escape char by "\hex_code", "\" also escaping
QString string_escape(const QString& src, const QString& chars_to_escape, const QChar& escape_char = '\\');

// restore all escape chains to real chars
QString string_unescape(const QString& src, const QChar& escape_char = '\\');

#endif	/* _STRING_ESCAPE_H */

