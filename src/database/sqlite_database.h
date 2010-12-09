#ifndef _SQLITE_DATABASE_H
#define _SQLITE_DATABASE_H

#include <QString>

#include "simple_database.h"

struct sqlite3;

class sqlite_database : public database
{
    public:
        sqlite_database(const QString& filename);
        ~sqlite_database();

        virtual result_set query(const QString& sql, size_t limit = 0);

    private:
        QString filename_;
        sqlite3 *handle_;
};

#endif
