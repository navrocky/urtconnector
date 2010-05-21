#ifndef _OUTPUT_SYSLOG_H
#define	_OUTPUT_SYSLOG_H

#include <string>

#include <cl/syslog/output.h>

namespace cl
{
namespace syslog
{

class message;

/*! \brief Operation System log output. */
class output_syslog : public output
{
public:
    output_syslog(const std::string& ident);
    ~output_syslog();
protected:
    void do_write(const message& msg);
private:
    std::string ident_;
};

}
}

#endif	/* _OUTPUT_SYSLOG_H */

