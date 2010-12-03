#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <QString>

class qexception: public std::runtime_error
{
public:
    explicit qexception(const QString& msg);
    explicit qexception(const char* msg);
    virtual ~qexception() throw ();

    QString message() const;
};

#endif
