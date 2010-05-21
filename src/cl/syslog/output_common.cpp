#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "common.h"
#include "output_common.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

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

std::string message_to_str(const message& msg)
{
    ptime tm = microsec_clock::local_time();
    return str(format("%1% (%3%)%2% %4%: %5%\n") % tm %
            level_to_str(msg.level) % msg.level %
            msg.module % msg.msg);

}

}
}
}
