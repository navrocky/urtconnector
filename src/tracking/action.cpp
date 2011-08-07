#include "action.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// action_class

action_class::action_class(const context_p& ctx,
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

action_t::action_t(const action_class_p& c)
: class_(c)
{
}

void action_t::assign(action_t* src)
{
    settings_t s;
    src->save(s);
    load(s);
}


}
