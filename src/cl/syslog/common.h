#ifndef _CL_SYSLOG_COMMON_H
#define	_CL_SYSLOG_COMMON_H

namespace cl
{
namespace syslog
{

/*! defined if thread logging needed */
#define USE_THREADS

/*! Log message level */
enum level_t
{
    harddebug,
    debug,
    info,
    warning,
    error,
    critical
};

const level_t default_level = debug;

}
}

#endif	/* _CL_SYSLOG_COMMON_H */

