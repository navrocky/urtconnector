#ifndef _CL_SYSLOG_MESSAGE_H
#define	_CL_SYSLOG_MESSAGE_H

#include <string>

#include <cl/syslog/common.h>

namespace cl
{
namespace syslog
{

/*! Syslog message */
class message
{
public:
    message();
    message(level_t level_, const std::string& module_, const std::string& msg_);
    message(const message& msg);

    level_t level;
    std::string module;
    std::string msg;
};

}
}

#endif	/* _LOG_MESSAGE_H */
