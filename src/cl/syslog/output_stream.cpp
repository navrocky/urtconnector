#include <string>

#include "common.h"
#include "message.h"
#include "output_stream.h"
#include "output_common.h"

using namespace std;

namespace cl
{
namespace syslog
{

output_stream::output_stream(std::ostream& stream)
: output(),
  stream_(stream)
{
}

void output_stream::do_write(const message& msg)
{
    stream_ << internal::message_to_str(msg);
}

}
}
