#ifndef _SIMPLE_DATABASE_H
#define _SIMPLE_DATABASE_H

#include <vector>
#include <string>
#include <QString>

class simple_database
{
    public:
        typedef std::vector< std::vector< std::string > > result_set;
        typedef std::vector< std::string > result_row;

        virtual ~simple_database() {};
        virtual result_set query(std::string sql, size_t limit = 0) = 0;
        virtual void populate() = 0;

        std::string ssencode_string(const std::string& str);
        std::string ssdecode_string(const std::string& str);
        QString qqencode_string(const QString& str);
        QString qqdecode_string(const QString& str);
        QString sqencode_string(const std::string& str);
        QString sqdecode_string(const std::string& str);

        static simple_database *db;
};

#endif
