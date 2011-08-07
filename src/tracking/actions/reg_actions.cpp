#include "reg_actions.h"

#include "../action.h"
#include "show_message_action.h"
#include "connect_action.h"

namespace tracking
{

action_factory_p reg_actions(const context_p& ctx)
{
    action_factory_p f(new action_factory(ctx));

    // register classes here
    f->reg<show_message_action_class>();
    f->reg<connect_action_class>();
    return f;
}

}