//#include <syslog.h>

#include <boost/format.hpp>

#include "message.h"
#include "output_syslog.h"

namespace cl
{
namespace syslog
{

// output_syslog::output_syslog(const std::string& ident)
// : ident_(ident)
// {
//     ::openlog(ident_.c_str(), 0, 0);
// }
// 
// output_syslog::~output_syslog()
// {
//     ::closelog();
// }
// 
// void output_syslog::do_write(const message& msg)
// {
//     int pri;
//     switch (msg.level)
//     {
//     case harddebug:
//     case debug:
//         pri = LOG_DEBUG;
//         break;
//     case info:
//         pri = LOG_INFO;
//         break;
//     case warning:
//         pri = LOG_WARNING;
//         break;
//     case error:
//         pri = LOG_ERR;
//         break;
//     case critical:
//         pri = LOG_CRIT;
//         break;
//     default:
//         pri = LOG_DEBUG;
//     };
// 
//     ::syslog(pri, "%s", boost::str(boost::format("[%1%] %2%") % msg.module % msg.msg).c_str());
// }

}
}
