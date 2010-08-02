#include "filter.h"

////////////////////////////////////////////////////////////////////////////////
// filter

filter::filter(filter_class_p fc)
: fc_(fc)
, enabled_(true)
{
}

void filter::set_name(const QString& name)
{
    name_ = name;
}

void filter::set_enabled(bool val)
{
    enabled_ = val;
}

////////////////////////////////////////////////////////////////////////////////
// filter_class

filter_class::filter_class(const QString& id, const QString& caption, const QString& description)
: id_(id)
, caption_(caption)
, description_(description)
{
}