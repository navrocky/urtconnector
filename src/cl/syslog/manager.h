#ifndef _CL_SYSLOG_MANAGER_H
#define	_CL_SYSLOG_MANAGER_H

#include <memory>

#include <cl/syslog/common.h>
#include <cl/syslog/output.h>
#include <cl/syslog/message.h>

namespace cl
{
namespace syslog
{

class manager
{
public:
    manager();
    ~manager();

    /*! Add new output to log. */
    void output_add(output_p out);

    /*! Write a message to log. */
    void write(const message& msg);

    /*! Checks that message level lv conform to current global message level. */
    bool level_check(level_t lv) const;
    
    /*! Change current global message level. */
    void level_set(level_t lv);

private:
    struct impl;
    std::auto_ptr<impl> impl_;
};

}
}

#endif	/* _LOG_MANAGER_H */
