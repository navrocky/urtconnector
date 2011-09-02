#include "select_server_action.h"

#include <common/server_id.h>
#include "../context.h"
#include "../tools.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// select_server_action_class

select_server_action_class::select_server_action_class(const context_p& ctx)
: action_class(ctx, "select", QObject::tr("Select founded server"),
               QObject::tr("Selects a founded server in the full server list."),
               QIcon("icons:zoom.png"))
{

}

action_p select_server_action_class::create()
{
    return action_p(new select_server_action(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// select_server_action

select_server_action::select_server_action(const action_class_p& c)
: action_t(c)
{
}

action_t::result_t select_server_action::execute()
{
    context_p ctx = get_class()->context();
    data_map_t::iterator it = ctx->data.find("server_id");
    if (it != ctx->data.end())
    {
        QString s = it.value();
        server_id id(s);
        ctx->select_server(id);
    }
    return r_continue;
}

QWidget* select_server_action::create_options_widget(QWidget* parent)
{
    return 0;
}

}

