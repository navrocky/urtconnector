#ifndef _SQLITE_DATABASE_H
#define _SQLITE_DATABASE_H

#include "database/simple_database.h"
#include "exception.h"
#include <string>
#include <sqlite3.h>

#define FOREVER for(;;)

class sqlite_database : public simple_database
{
    public:
        sqlite_database(std::string filename);
        ~sqlite_database();

        virtual result_set query(std::string sql, size_t limit = 0);
        virtual void populate();

    private:
        std::string m_filename;
        sqlite3 *m_handle;
};

#endif
