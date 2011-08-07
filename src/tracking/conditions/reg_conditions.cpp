#include "reg_conditions.h"

#include "timer_condition.h"
#include "server_filter_condition.h"

namespace tracking
{

condition_factory_p reg_conditions(const context_p& ctx)
{
    condition_factory_p f(new condition_factory(ctx));

    // register classes here
    f->reg<timer_condition_class>();
    f->reg<server_filter_condition_class>();
    return f;
}

}