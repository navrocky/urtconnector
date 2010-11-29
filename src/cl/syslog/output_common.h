#ifndef _OUTPUT_COMMON_H
#define	_OUTPUT_COMMON_H

#include <string>

#include <cl/syslog/message.h>

namespace cl
{
namespace syslog
{

class thread_info;

namespace internal
{


/*! Take a string message representation, commonly used in stream and file outputs. */
std::string message_to_str(const message& msg, const thread_info& info);

}
}
}

#endif	/* _OUTPUT_COMMON_H */

