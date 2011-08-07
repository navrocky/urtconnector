#ifndef SCOPED_TOOLS_H
#define SCOPED_TOOLS_H

#include <boost/call_traits.hpp>

/*! Use this scoped class to temporarely change some value and restore old value
    when scope out.

 \code
 bool flag;
 {
     scoped_value_change<bool> s(flag, true, flag);
     ...
     // or
     scoped_value_change<bool> s(flag, true);
     ...

     // or even
     SCOPE_COCK_FLAG(flag);
     ...
 }
 \endcode
 */

/*! Temporarely set a flag to true and return old flag value when the scope is out.*/
#define SCOPE_COCK_FLAG(flag) scoped_value_change<bool> __sv_##flag(flag, true)

/*! Temporarely set a flag to false and return old flag value when the scope is out.*/
#define SCOPE_RESET_FLAG(flag) scoped_value_change<bool> __sv_##flag(flag, false)

template <typename T>
class scoped_value_change
{
public:
    typedef typename boost::call_traits<T>::reference reference;
    typedef typename boost::call_traits<T>::param_type param;

    scoped_value_change(reference value, param in)
    : value_(value), out_(value)
    {
        value_ = in;
    }

    scoped_value_change(reference value, param in, param out)
    : value_(value), out_(out)
    {
        value_ = in;
    }

    ~scoped_value_change()
    {
        value_ = out_;
    }
private:
    reference value_;
    T out_;
};

#endif