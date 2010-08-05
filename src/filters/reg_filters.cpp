#include <boost/make_shared.hpp>
#include "filter_factory.h"

#include "composite_filter.h"

#include "reg_filters.h"

void register_filters(filter_factory_p f)
{
    f->add_class(boost::make_shared<composite_filter_class>());

}