#ifndef _OUTPUT_STREAM_H
#define	_OUTPUT_STREAM_H

#include <ostream>

#include <cl/syslog/output.h>

namespace cl
{
namespace syslog
{

class message;

/*! \brief Stream log output. */
class output_stream : public output
{
public:
    output_stream(std::ostream& stream);
protected:
    void do_write(const message& msg);
private:
    std::ostream& stream_;
};

}
}

#endif	/* _OUTPUT_STREAM_H */

