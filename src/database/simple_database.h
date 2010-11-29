#ifndef _SIMPLE_DATABASE_H
#define _SIMPLE_DATABASE_H

#include <vector>
#include <string>

class simple_database
{
    public:
        typedef std::vector< std::vector< std::string > > result_set;
        typedef std::vector< std::string > result_row;

        virtual ~simple_database() {};
        virtual result_set query(std::string sql, size_t limit = 0) = 0;
        static simple_database *db;
};

#endif
