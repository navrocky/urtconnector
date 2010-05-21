#include <boost/thread/mutex.hpp>

#include "manager.h"

using namespace boost;

namespace cl
{
namespace syslog
{

struct manager::impl
{
    typedef std::vector<output_p> output_list;
    output_list outputs_;
    level_t level_;
    mutex mutex_;
};

manager::manager()
: impl_(new impl())
{
    impl_->level_ = default_level;
}

manager::~manager()
{
}

void manager::output_add(output_p out)
{
    impl_->outputs_.push_back(out);
}

bool manager::level_check(level_t lv) const
{
    return lv >= impl_->level_;
}

void manager::write(const message& msg)
{
    mutex::scoped_lock lock(impl_->mutex_);
    for (impl::output_list::iterator i = impl_->outputs_.begin(); i != impl_->outputs_.end(); i++)
        (*i)->write(msg);
}

void manager::level_set(level_t lv)
{
    impl_->level_ = lv;
}

}
}