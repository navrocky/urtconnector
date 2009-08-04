#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <QString>

class Exception: public std::runtime_error
{
public:
    explicit Exception(const QString& msg);
    explicit Exception(const char* msg);
    virtual ~Exception() throw ();

    QString message() const;
};

#endif
