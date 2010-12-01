#include "database/sqlite_database.h"
#include <fstream>

sqlite_database::sqlite_database(std::string filename)
{
    m_filename = filename;
    if (sqlite3_open_v2(m_filename.c_str(), &m_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
    {
        throw qexception((std::string("Cannot open SQLite database file: ")+sqlite3_errmsg(m_handle)).c_str());
    }
    simple_database::db = this;
    query("PRAGMA auto_vacuum = 1");
    populate();
}
sqlite_database::~sqlite_database()
{
    sqlite3_close(m_handle);
}

sqlite_database::result_set sqlite_database::query(std::string sql, size_t limit)
{
    sqlite3_stmt *statement;
    result_set answer;
    if (sqlite3_prepare_v2(m_handle, sql.c_str(), -1, &statement, 0) == SQLITE_OK)
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
                    row.push_back((const char*)sqlite3_column_text(statement,i));
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
        throw qexception((std::string("Cannot execute query: ")+sqlite3_errmsg(m_handle)).c_str());
    }
    if (sqlite3_finalize(statement)!=SQLITE_OK)
    {
        throw qexception((std::string("Unable to finalize query: ")+sqlite3_errmsg(m_handle)).c_str());
    }
    return answer;
}

void sqlite_database::populate()
{
    query
    (
        "CREATE TABLE IF NOT EXISTS all_state"
        "("
        "    id INTEGER,"
        "    address VARCHAR(255),"
        "    name VARCHAR(255),"
        "    gametype INTEGER,"
        "    map VARCHAR(255),"
        "    mapurl VARCHAR(255),"
        "    maxplayercount VARCHAR(255),"
        "    mode INTEGER,"
        "    ping INTEGER,"
        "    country VARCHAR(255),"
        "    countrycode VARCHAR(255),"
        "    info TEXT,"
        "    PRIMARY KEY(id)"
        ");"
    );
    query
    (
        "CREATE TABLE IF NOT EXISTS favs_state"
        "("
        "    id INTEGER,"
        "    address VARCHAR(255),"
        "    name VARCHAR(255),"
        "    gametype INTEGER,"
        "    map VARCHAR(255),"
        "    mapurl VARCHAR(255),"
        "    maxplayercount VARCHAR(255),"
        "    mode INTEGER,"
        "    ping INTEGER,"
        "    country VARCHAR(255),"
        "    countrycode VARCHAR(255),"
        "    info TEXT,"
        "    PRIMARY KEY(id)"
        ");"
    );

}
