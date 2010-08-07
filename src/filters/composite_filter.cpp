#include <QObject>
#include <QComboBox>
#include <boost/make_shared.hpp>

#include "composite_filter.h"
#include "filter_edit_widget.h"

Q_DECLARE_METATYPE(composite_filter::operation_t)

////////////////////////////////////////////////////////////////////////////////
// composite_filter_class

composite_filter_class::composite_filter_class()
: filter_class(get_id(), QObject::tr("Composite filter"),
               QObject::tr("Composite child filters using logical operations."))
{}

const char* composite_filter_class::get_id()
{
    return "composite";
}

filter_p composite_filter_class::create_filter()
{
    return boost::make_shared<composite_filter>(shared_from_this());
}

QWidget* composite_filter_class::create_quick_opts_widget(filter_p f)
{
    return new composite_filter_quick_opt_widget(f);
}


////////////////////////////////////////////////////////////////////////////////
// composite_filter

composite_filter::composite_filter(filter_class_p fc)
: filter(fc)
, operation_(op_and)
{
}

composite_filter::~composite_filter()
{
    delete combo_;
}

bool composite_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    switch (operation_)
    {
        case op_and:
        {
            foreach (filter_p f, filters_)
                if (!f->filter_server(si))
                    return false;
            return true;
        }
        case op_or:
        {
            foreach (filter_p f, filters_)
                if (f->filter_server(si))
                    return true;
            return filters_.size() > 0;
        }

        default:;
    }
    return true;
}

void composite_filter::set_operation(operation_t op)
{
    if (operation_ == op)
        return;
    operation_ = op;
    emit changed_signal();
}

void composite_filter::add_filter(filter_p f)
{
    filters_.push_back(f);
    connect(f.get(), SIGNAL(changed_signal()), SLOT(child_filter_changed()));
    emit changed_signal();
}

void composite_filter::remove_filter(filter_p f)
{
    disconnect(f.get(), SIGNAL(changed_signal()), this, SLOT(child_filter_changed()));
    filters_t::iterator it = std::find(filters_.begin(), filters_.end(), f);
    assert(it != filters_.end());
    filters_.erase(it);
    emit changed_signal();
}

void composite_filter::child_filter_changed()
{
    emit changed_signal();
}

////////////////////////////////////////////////////////////////////////////////
// composite_filter_quick_opt_widget

composite_filter_quick_opt_widget::composite_filter_quick_opt_widget(filter_p f)
: filter_(f)
{
    addItem(tr("AND"), QVariant::fromValue(composite_filter::op_and));
    addItem(tr("OR"), QVariant::fromValue(composite_filter::op_or));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
