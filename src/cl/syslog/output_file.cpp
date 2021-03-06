#include <boost/filesystem.hpp>

#include "output_common.h"
#include "output_file.h"

using namespace std;

using namespace boost::filesystem;

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
        throw std::runtime_error("Can't open file " + file_name);
}

void output_file::do_write(const message& msg, const thread_info& info)
{
    f_ << internal::message_to_str(msg, info);
    f_.flush();
}

}
}