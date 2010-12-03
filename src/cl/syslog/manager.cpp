#include <vector>
#include <map>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "thread_info.h"
#include "manager.h"

using namespace boost;

namespace cl
{
namespace syslog
{

////////////////////////////////////////////////////////////////////////////////
// impl

struct manager::impl
{
    impl()
    : level_(default_level)
    , thread_num_(0)
    {}

    typedef std::vector<output_p> output_list;
    typedef std::map<boost::thread::id, thread_info> threads_t;
    output_list outputs_;
    level_t level_;
    threads_t threads_;
    int thread_num_;
    recursive_mutex mutex_;
};

////////////////////////////////////////////////////////////////////////////////
// manager

manager::manager()
: impl_(new impl())
{
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
    using namespace boost::posix_time;

    boost::thread::id id = boost::this_thread::get_id();

    boost::unique_lock<boost::recursive_mutex> lock(impl_->mutex_);

    impl::threads_t& threads = impl_->threads_;
    
    impl::threads_t::iterator it = threads.find(id);
    thread_info* inf;

    if (it == threads.end())
    {
        thread_info info;
        info.num = impl_->thread_num_++;
        info.time = microsec_clock::local_time();
        threads[id] = info;
        inf = &(threads[id]);
    } else
    {
        inf = &(it->second);
    }
        
    inf->prev_time = inf->time;
    inf->time = microsec_clock::local_time();

    if (it == threads.end())
        write(message(debug, "syslog",
            (boost::format("Thread #%1% with id=%2%") % inf->num % id).str()));

    for (impl::output_list::iterator i = impl_->outputs_.begin(); i != impl_->outputs_.end(); i++)
        (*i)->write(msg, *inf);
}

void manager::level_set(level_t lv)
{
    impl_->level_ = lv;
}

}
}
