#ifndef _SIMPLE_DATABASE_H
#define _SIMPLE_DATABASE_H

#include <QList>
#include <QString>

class database
{
public:
    typedef QList<QString> result_row;
    typedef QList< result_row > result_set;

    database();
    virtual ~database();
    virtual result_set query(const QString& sql, size_t limit = 0) = 0;

    static database* instance() {return instance_;}

    static QString qqencode_string(const QString& str);
    static QString qqdecode_string(const QString& str);
private:
    static database* instance_;
};

#endif
