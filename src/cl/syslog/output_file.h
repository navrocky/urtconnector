#ifndef _OUTPUT_FILE_H
#define	_OUTPUT_FILE_H

#include <fstream>

#include <cl/syslog/output.h>

namespace cl
{
namespace syslog
{

class message;

/*! \brief File log output. */
class output_file : public output
{
public:
    output_file(const std::string& file_name, bool append = false);
protected:
    void do_write(const message& msg);
private:
    std::fstream f_;
};

}
}

#endif	/* _OUTPUT_FILE_H */

