#include <cl/except/error.h>
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

QByteArray filter::save()
{
    return QByteArray();
}

void filter::load(const QByteArray& ba, filter_factory_p factory)
{
}


////////////////////////////////////////////////////////////////////////////////
// filter_class

filter_class::filter_class(const QString& id, const QString& caption, 
                           const QString& description, const QIcon& icon)
: id_(id)
, caption_(caption)
, description_(description)
, icon_(icon)
{
    if (icon_.isNull())
        icon_ = QIcon(":/icons/icons/view-filter.png");
}

bool filter_class::has_additional_options()
{
    return false;
}

QWidget* filter_class::create_quick_opts_widget(filter_p f)
{
    return NULL;
}
