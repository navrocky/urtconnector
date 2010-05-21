#include "message.h"

namespace cl
{
namespace syslog
{

message::message()
: level(default_level)
{
}

message::message(level_t level_, const std::string& module_, const std::string& msg_)
: level(level_), module(module_), msg(msg_)
{
}

message::message(const message& m)
: level(m.level), module(m.module), msg(m.msg)
{
}

}
}