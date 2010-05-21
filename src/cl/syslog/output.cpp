#include "output.h"

namespace cl
{
namespace syslog
{

output::output()
{
}

output::~output()
{
}

void output::filter_add(filter_t flt)
{
    filters_.push_back(flt);
}

bool output::check(const message& msg)
{
    for (filter_list::iterator i = filters_.begin(); i != filters_.end(); i++)
        if ( !(*i)(msg) )
            return false;
    return true;
}

void output::write(const message& msg)
{
    if (check(msg))
        do_write(msg);
}

}
}
