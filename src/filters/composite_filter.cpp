#include <QObject>
#include <QComboBox>
//#include <boost/make_shared.hpp>
#include <cl/except/error.h>

#include "composite_filter.h"
#include "filter_edit_widget.h"
#include "tools.h"

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
    return filter_p(new composite_filter(shared_from_this()));
}

QWidget* composite_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
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
                if (f->enabled() && !f->filter_server(si))
                    return false;
            return true;
        }
        case op_or:
        {
            foreach (filter_p f, filters_)
                if (f->enabled() && f->filter_server(si))
                    return true;
            return filters_.size() > 0 ? false : true;
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

QByteArray composite_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << (qint32)operation_;
    
    // save child filters
    ds << (qint32)(filters_.size());
    foreach (filter_p f, filters_)
        ds << filter_save(f);
    return res;
}

void composite_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    assert(filters_.size() == 0);

    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    qint32 v;
    ds >> v;
    operation_ = (operation_t)v;

    qint32 sz;
    ds >> sz;
    for (int i = 0; i < sz; i++)
    {
        QByteArray ba2;
        ds >> ba2;
        filter_p f = filter_load(ba2, factory);
        add_filter(f);
    }
}


////////////////////////////////////////////////////////////////////////////////
// composite_filter_quick_opt_widget

composite_filter_quick_opt_widget::composite_filter_quick_opt_widget(filter_p f)
: filter_(f)
, block_filter_change_(false)
, block_combo_change_(false)
{
    addItem(tr("AND"), QVariant::fromValue((int)composite_filter::op_and));
    addItem(tr("OR"), QVariant::fromValue((int)composite_filter::op_or));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(combo_changed()));
}

void composite_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;
    block_combo_change_ = true;
    composite_filter* cf = qobject_cast<composite_filter*>(filter_.get());
    int i = findData((int)cf->operation());
    setCurrentIndex(i);
    block_combo_change_ = false;
}

void composite_filter_quick_opt_widget::combo_changed()
{
    if (block_combo_change_)
        return;
    block_filter_change_ = true;
    composite_filter* cf = qobject_cast<composite_filter*>(filter_.get());
    cf->set_operation((composite_filter::operation_t)(itemData(currentIndex())
        .value<int>()));
    block_filter_change_ = false;
}

