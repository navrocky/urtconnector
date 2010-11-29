#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "common.h"
#include "output_common.h"
#include "thread_info.h"

namespace cl
{
namespace syslog
{
namespace internal
{

std::string level_to_str(syslog::level_t lv)
{
    switch (lv)
    {
    case syslog::harddebug:
        return "hard";
    case syslog::debug:
        return "dbg ";
    case syslog::info:
        return "info";
    case syslog::warning:
        return "warn";
    case syslog::error:
        return "err ";
    case syslog::critical:
        return "crit";
    }
    return "unkn";
}

std::string message_to_str(const message& msg, const thread_info& info)
{
    using namespace boost::posix_time;
    int delta = int((info.time - info.prev_time).total_microseconds());
    return (boost::format("%1% D:%2$6d TH:%3% (%4%)%5% %6%: %7%\n") % info.time
            % delta % info.num
            % level_to_str(msg.level) % msg.level %
            msg.module % msg.msg ).str();
}

}
}
}
