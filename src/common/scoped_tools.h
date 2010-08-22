#pragma once

/*! Use this scoped class to temporarely change some value and restore old value
    when scope out.

 \code
 bool flag;
 {
     scoped_value_change s(flag, true, flag)
     ...
 }
 \endcode
 */
template <typename T>
class scoped_value_change
{
public:
    scoped_value_change(T& value, const T& in, const T& out)
    : value_(value), out_(out)
    {
        value_ = in;
    }

    ~scoped_value_change()
    {
        value_ = out_;
    }
private:
    T& value_;
    T out_;
};
