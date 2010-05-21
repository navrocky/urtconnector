#include <boost/filesystem.hpp>

#include <cl/except/error.h>

#include "output_common.h"
#include "output_file.h"

using namespace std;

using namespace boost::filesystem;
using namespace cl::except;

namespace cl
{
namespace syslog
{

output_file::output_file(const std::string& file_name, bool append)
        : output()
{
    path fp(file_name);
    create_directories(fp.parent_path());

    if (append)
        f_.open(file_name.c_str(), fstream::out | fstream::app);
    else
        f_.open(file_name.c_str(), fstream::out | fstream::trunc);

    if (!f_.good())
        throw cl::except::error("Can't open file " + file_name);
}

void output_file::do_write(const message& msg)
{
    f_ << internal::message_to_str(msg);
    f_.flush();
}

}
}