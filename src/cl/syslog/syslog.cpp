#include <boost/pool/detail/singleton.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "syslog.h"
#include "message.h"

namespace cl
{
namespace syslog
{

manager& logman()
{
    return boost::details::pool::singleton_default<manager>::instance();
}

template<>
const autolog& operator<< <level_t>(const autolog& al, const level_t& value)
{
    al.set_level(value);
    return al;
}

template<>
autolog operator<< <level_t>(const module& m, const level_t& value)
{
    return autolog(message(value, m.module_name(), std::string()));
}

////////////////////////////////////////////////////////////////////////////////
// autolog_ptr

autolog_ptr::autolog_ptr()
: al_(NULL)
{}

autolog_ptr::~autolog_ptr()
{
    delete al_;
}

autolog& autolog_ptr::change(autolog* al)
{
    delete al_;
    al_ = al;
    return *al_;
}

////////////////////////////////////////////////////////////////////////////////
// autolog
////////////////////////////////////////////////////////////////////////////////


autolog::autolog(const message& msg)
: msg_(new message(msg))
, ins_num_(0)
{
}

autolog::autolog(const autolog& lg)
: msg_(lg.msg_)
, ins_num_(lg.ins_num_)
{
}

autolog::autolog(const module& m)
: msg_(new message)
, ins_num_(0)
{
    msg_->module = m.module_name();
}

autolog::~autolog()
{
    // replace % insertions
    if (ins_num_ > 0)
        boost::replace_all(msg_->msg, "\\%", "%");
    
    if (!msg_->msg.empty())
        logman().write(*msg_);
}

void autolog::append_to_msg(const std::string& str) const
{
    msg_->msg += str;
}

void autolog::insert_to_msg(const std::string& str) const
{
    autolog* al = const_cast<autolog*>(this);
    al->ins_num_++;
    boost::replace_all(msg_->msg, "%" + boost::lexical_cast<std::string>(ins_num_), str);
}

void autolog::set_level(level_t lv) const
{
    msg_->level = lv;
}

////////////////////////////////////////////////////////////////////////////////
// module
////////////////////////////////////////////////////////////////////////////////


module::module(const std::string& module_name)
: module_name_(module_name)
{
}

module::module(const char* module_name)
: module_name_(module_name)
{
}

}
}
