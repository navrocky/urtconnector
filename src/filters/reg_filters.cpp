#include "filter_factory.h"

#include "pointers.h"
#include "filter.h"
#include "composite_filter.h"
#include "game_type_filter.h"
#include "hide_empty_filter.h"
#include "hide_full_filter.h"
#include "regexp_filter.h"
#include "online_clans_filter.h"
#include "hide_private_filter.h"
#include "weapon_filter.h"
#include "ping_filter.h"
#include "custom_filter.h"
#include "player_filter.h"

#include "reg_filters.h"

template <typename T>
void reg(filter_factory_p f)
{
    f->add_class(filter_class_p(new T));
}

void register_filters(filter_factory_p f)
{
    reg<regexp_filter_class>(f);
    reg<composite_filter_class>(f);
    reg<game_type_filter_class>(f);
    reg<hide_empty_filter_class>(f);
    reg<hide_full_filter_class>(f);
    reg<online_clans_filter_class>(f);
    reg<hide_private_filter_class>(f);
    reg<weapon_filter_class<false> >(f); // allow weapon filter
    reg<weapon_filter_class<true> >(f);  // forbid weapon filter
    reg<ping_filter_class>(f);
    reg<custom_filter_class>(f);
    reg<player_filter_class>(f);
}
