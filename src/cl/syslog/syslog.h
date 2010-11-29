#ifndef _SYSLOG_H
#define	_SYSLOG_H

#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <cl/syslog/common.h>
#include <cl/syslog/manager.h>

#define SYSLOG_MODULE(a) const cl::syslog::module ___syslog(#a);

#ifdef SYSLOG_WRITE_FUNCTIONS

#define LOG_CRIT if (cl::syslog::logman().level_check(cl::syslog::critical)) ___syslog << cl::syslog::critical << __FILE__ << "@" << __LINE__ << " "
#define LOG_ERR if (cl::syslog::logman().level_check(cl::syslog::error)) ___syslog << cl::syslog::error << __FILE__ << "@" << __LINE__ << " "
#define LOG_WARN if (cl::syslog::logman().level_check(cl::syslog::warning)) ___syslog << cl::syslog::warning << __FILE__ << "@" << __LINE__ << " "
#define LOG_INFO if (cl::syslog::logman().level_check(cl::syslog::info)) ___syslog << cl::syslog::info << __FILE__ << "@" << __LINE__ << " "
#define LOG_DEBUG if (cl::syslog::logman().level_check(cl::syslog::debug)) ___syslog << cl::syslog::debug << __FILE__ << "@" << __LINE__ << " "
#define LOG_HARD if (cl::syslog::logman().level_check(cl::syslog::harddebug)) ___syslog << cl::syslog::harddebug << __FILE__ << "@" << __LINE__ << " "

#else

#define LOG_CRIT if (cl::syslog::logman().level_check(cl::syslog::critical)) ___syslog << cl::syslog::critical
#define LOG_ERR if (cl::syslog::logman().level_check(cl::syslog::error)) ___syslog << cl::syslog::error
#define LOG_WARN if (cl::syslog::logman().level_check(cl::syslog::warning)) ___syslog << cl::syslog::warning
#define LOG_INFO if (cl::syslog::logman().level_check(cl::syslog::info)) ___syslog << cl::syslog::info
#define LOG_DEBUG if (cl::syslog::logman().level_check(cl::syslog::debug)) ___syslog << cl::syslog::debug
#define LOG_HARD if (cl::syslog::logman().level_check(cl::syslog::harddebug)) ___syslog << cl::syslog::harddebug

#endif

#ifdef MSVC
#define CHECKPOINT std::cerr << __FUNCSIG__ << "| thread:" << boost::this_thread::get_id() << std::endl;
#else
#define CHECKPOINT std::cerr << __PRETTY_FUNCTION__ << "| thread:" << boost::this_thread::get_id() << std::endl;
#endif
//#define CHECKPOINT

/*!
*/
namespace cl
{
namespace syslog
{

class autolog;

class autolog_ptr
{
public:
    autolog_ptr();
    ~autolog_ptr();
    autolog& change(autolog*);
public:
    autolog* al_;
};

#define LOG_EXIT_WARN \
    cl::syslog::autolog_ptr ___scope_autolog;\
    if (cl::syslog::logman().level_check(cl::syslog::warning)) \
        ___scope_autolog.change(new cl::syslog::autolog(___syslog)) << cl::syslog::warning

#define LOG_EXIT_INFO \
    cl::syslog::autolog_ptr ___scope_autolog;\
    if (cl::syslog::logman().level_check(cl::syslog::info)) \
        ___scope_autolog.change(new cl::syslog::autolog(___syslog)) << cl::syslog::info

#define LOG_EXIT_DEBUG \
    cl::syslog::autolog_ptr ___scope_autolog;\
    if (cl::syslog::logman().level_check(cl::syslog::debug)) \
        ___scope_autolog.change(new cl::syslog::autolog(___syslog)) << cl::syslog::debug

#define LOG_EXIT_HARD \
    cl::syslog::autolog_ptr ___scope_autolog;\
    if (cl::syslog::logman().level_check(cl::syslog::harddebug)) \
        ___scope_autolog.change(new cl::syslog::autolog(___syslog)) << cl::syslog::harddebug

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
    autolog(const module& m);
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
