#include <sqlite3.h>
#include <QObject>

#include <common/exception.h>
#include <common/str_convert.h>

#include "sqlite_database.h"

#define FOREVER for(;;)

template <typename T>
QString utf16_to_qstr(T src)
{
    return QString::fromUtf16((const ushort*)src);
}

sqlite_database::sqlite_database(const QString& filename)
: filename_(filename)
, handle_(0)
{
    if (sqlite3_open_v2(filename_.toUtf8().data(), &handle_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0) != SQLITE_OK)
        throw qexception(QObject::tr("Cannot open SQLite database file: %1")
                         .arg(utf16_to_qstr(sqlite3_errmsg16(handle_))));
    query("PRAGMA auto_vacuum = 1");
}
sqlite_database::~sqlite_database()
{
    if (handle_)
        sqlite3_close(handle_);
}

database::result_set sqlite_database::query(const QString& sql, size_t limit)
{
    sqlite3_stmt *statement;
    result_set answer;
    if (sqlite3_prepare16_v2(handle_, sql.utf16(), -1, &statement, 0) == SQLITE_OK)
    {
        size_t cols = sqlite3_column_count(statement);
        int result_code = 0;

        bool we_need_to_stop = (limit>0) ? true : false;
        FOREVER
        {
            result_code = sqlite3_step(statement);
            if (result_code == SQLITE_ROW)
            {
                if (we_need_to_stop)
                {
                    if (limit > 0)
                    {
                        --limit;
                    }
                    else
                    {
                        break;
                    }
                }
                result_row row;
                for (size_t i = 0; i<cols; ++i)
                {
                    row.push_back(utf16_to_qstr(sqlite3_column_text16(statement,i)));
                }
                answer.push_back(row);
            }
            else
            {
                sqlite3_reset(statement);
                break;
            }
        }
    }
    else
    {
        throw qexception(QObject::tr("Cannot execute query: %1")
                         .arg(utf16_to_qstr(sqlite3_errmsg16(handle_))));
    }
    if (sqlite3_finalize(statement)!=SQLITE_OK)
    {
        throw qexception(QObject::tr("Unable to finalize query: %1")
                         .arg(utf16_to_qstr(sqlite3_errmsg16(handle_))));
    }
    return answer;
}
