#include <boost/make_shared.hpp>
#include "filter_factory.h"

#include "composite_filter.h"
#include "game_type_filter.h"
#include "hide_empty_filter.h"
#include "hide_full_filter.h"
#include "regexp_filter.h"
#include "online_clans_filter.h"

#include "reg_filters.h"

void register_filters(filter_factory_p f)
{
    f->add_class(boost::make_shared<regexp_filter_class>());
    f->add_class(boost::make_shared<composite_filter_class>());
    f->add_class(boost::make_shared<game_type_filter_class>());
    f->add_class(boost::make_shared<hide_empty_filter_class>());
    f->add_class(boost::make_shared<hide_full_filter_class>());
    f->add_class(boost::make_shared<online_clans_filter_class>());
}