#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <cl/syslog/common.h>
#include <cl/syslog/manager.h>


#define SYSLOG_MODULE(a) const cl::syslog::module __syslog(a);

#define LOG_CRIT if (cl::syslog::logman().level_check(cl::syslog::critical)) __syslog << cl::syslog::critical
#define LOG_ERR if (cl::syslog::logman().level_check(cl::syslog::error)) __syslog << cl::syslog::error
#define LOG_WARN if (cl::syslog::logman().level_check(cl::syslog::warning)) __syslog << cl::syslog::warning
#define LOG_INFO if (cl::syslog::logman().level_check(cl::syslog::info)) __syslog << cl::syslog::info
#define LOG_DEBUG if (cl::syslog::logman().level_check(cl::syslog::debug)) __syslog << cl::syslog::debug
#define LOG_HARD if (cl::syslog::logman().level_check(cl::syslog::harddebug)) __syslog << cl::syslog::harddebug

namespace cl
{
namespace syslog
{


class module
{
public:
    module(const std::string& module_name);
    module(const char* module_name);

    const std::string& module_name() const {return module_name_;}
private:
    std::string module_name_;
};

class autolog
{
public:
    autolog(const message& msg);
    autolog(const autolog& lg);
    ~autolog();

    void append_to_msg(const std::string& str) const;
    void insert_to_msg(const std::string& str) const;
    void set_level(level_t lv) const;
private:
    boost::shared_ptr<message> msg_;
    int ins_num_;
};

template <typename T>
const autolog& operator<<(const autolog& al, const T& value)
{
    al.append_to_msg(boost::lexical_cast<std::string, T>(value));
    return al;
}

template<>
const autolog& operator<< <level_t>(const autolog& al, const level_t& value);

template <typename T>
autolog operator<<(const module& m, const T& value)
{
    return autolog(message(default_level, m.module_name(), boost::lexical_cast<std::string, T>(value)));
}

/*! boost::format like functionality.

 \code
 // %1, %2... is a insertions to format string
 LOG_DEBUG << "Warning %1 from %2", 1, "here";

 // how to insert % in string
 LOG_DEBUG << "Warning %1 from %2 and this is \\%", 1, "here";
 \endcode
 */
template <typename T>
const autolog& operator,(const autolog& al, const T& value)
{
    al.insert_to_msg(boost::lexical_cast<std::string, T>(value));
    return al;
}

template<>
autolog operator<< <level_t>(const module& m, const level_t& value);


/*! Access to global log manager. */
manager& logman();

}
}


#endif	/* _SYSLOG_H */

