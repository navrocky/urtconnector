#include "condition.h"

#include <cassert>

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// condition_class

condition_class::condition_class(const context_p& ctx,
                                 const QString& id,
                                 const QString& caption,
                                 const QString& description,
                                 const QIcon& icon)
: ctx_(ctx)
, id_(id)
, caption_(caption)
, description_(description)
, icon_(icon)
{
}

////////////////////////////////////////////////////////////////////////////////
// condition

condition_t::condition_t(const condition_class_p& c)
: class_(c)
, started_(false)
, is_start_needed_(false)
{
}

void condition_t::stop()
{
    assert(started_);
    started_ = false;
    do_stop();
    emit changed();
}

void condition_t::start()
{
    is_start_needed_ = false;
    assert(!started_);
    do_start();
    started_ = true;
    emit changed();
}

void condition_t::save(settings_t& s)
{
    s["start_needed"] = started_;
}

void condition_t::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("start_needed");
    if (it != s.end())
        is_start_needed_ = it.value().toBool();
}

void condition_t::restart()
{
    stop();
    start();
}

void condition_t::trigger()
{
    emit triggered();
}

void condition_t::assign(condition_t* src)
{
    settings_t s;
    src->save(s);
    load(s);
}

}
