#ifndef _THREAD_INFO_H
#define	_THREAD_INFO_H

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace cl
{
namespace syslog
{

class thread_info
{
public:
    int num;
    boost::posix_time::ptime prev_time;
    boost::posix_time::ptime time;
};

}
}



#endif	/* _THREAD_INFO_H */

