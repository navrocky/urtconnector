#include "filter.h"

////////////////////////////////////////////////////////////////////////////////
// filter

filter::filter(filter_class_p fc)
: QObject()
, fc_(fc)
, enabled_(true)
{
}

void filter::set_name(const QString& name)
{
    name_ = name;
}

void filter::set_enabled(bool val)
{
    if (enabled_ == val)
        return;
    enabled_ = val;
    emit changed_signal();
}

////////////////////////////////////////////////////////////////////////////////
// filter_class

filter_class::filter_class(const QString& id, const QString& caption, const QString& description)
: id_(id)
, caption_(caption)
, description_(description)
{
}

bool filter_class::has_additional_options()
{
    return false;
}

QWidget* filter_class::create_quick_opts_widget(filter_p f)
{
    return NULL;
}
