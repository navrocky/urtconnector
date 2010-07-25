#include "filter.h"

////////////////////////////////////////////////////////////////////////////////
// filter

filter::filter(filter_class_p fc)
: fc_(fc)
{
}

void filter::set_uid(const QString& uid)
{
    uid_ = uid;
}


////////////////////////////////////////////////////////////////////////////////
// filter_class

filter_class::filter_class(const QString& id, const QString& caption, const QString& description)
: id_(id)
, caption_(caption)
, description_(description)
{
}