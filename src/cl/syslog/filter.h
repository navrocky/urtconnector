#ifndef _CL_SYSLOG_FILTER_H
#define	_CL_SYSLOG_FILTER_H

#include <boost/function.hpp>

namespace cl
{
namespace syslog
{

class message;

typedef boost::function<bool(const message&)> filter_t;

}
}

#endif	/* _LOG_FILTER_H */

