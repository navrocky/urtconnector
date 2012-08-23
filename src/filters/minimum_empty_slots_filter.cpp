#include "minimum_empty_slots_filter.h"

#include <QObject>
#include <QSpinBox>
#include <QHBoxLayout>

#include <cl/except/error.h>
#include <common/scoped_tools.h>
#include <common/tools.h>

#include "filter_edit_widget.h"
#include "tools.h"

////////////////////////////////////////////////////////////////////////////////
// minimum_empty_slots_filter_class

minimum_empty_slots_filter_class::minimum_empty_slots_filter_class()
: filter_class(get_id(), QObject::tr("Minimum empty slots"),
    QObject::tr("Shows servers with a specified minimum number of empty slots."))
{}

const char* minimum_empty_slots_filter_class::get_id()
{
    return "min_empty_slots";
}

filter_p minimum_empty_slots_filter_class::create_filter()
{
    return filter_p(new minimum_empty_slots_filter(shared_from_this()));
}

QWidget* minimum_empty_slots_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new min_empty_slots_filter_quick_opt_widget(f, parent);
}


////////////////////////////////////////////////////////////////////////////////
// minimum_empty_slots_filter

minimum_empty_slots_filter::minimum_empty_slots_filter(filter_class_p fc)
: filter(fc)
, minimum_(2)
{
}

bool minimum_empty_slots_filter::filter_server(const server_info& si, filter_context& ctx)
{
    if (!enabled())
        return true;

    return si.public_slots() - si.players.size() >= minimum_;
}

void minimum_empty_slots_filter::set_minimum_empty_slots(int val)
{
    if (minimum_ == val)
        return;
    minimum_ = val;
    emit changed_signal();
}

QByteArray minimum_empty_slots_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << (qint32)minimum_;
    return res;
}

void minimum_empty_slots_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    qint32 v;
    ds >> v;
    minimum_ = v;
}


////////////////////////////////////////////////////////////////////////////////
// min_empty_slots_filter_quick_opt_widget

min_empty_slots_filter_quick_opt_widget::min_empty_slots_filter_quick_opt_widget(filter_p f, QWidget* parent)
: QWidget(parent)
, filter_(f)
, block_filter_change_(false)
, block_ctl_change_(false)
{
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    spin_ = new QSpinBox(this);
    spin_->setToolTip(tr("Minimum empty slots on server"));
    lay->addWidget(spin_);

    lay->addStretch();

    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(spin_, SIGNAL(valueChanged(int)), SLOT(spin_changed()));
}

void min_empty_slots_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;
    scoped_value_change<bool> sv(block_ctl_change_, true, false);
    minimum_empty_slots_filter* cf = qobject_cast<minimum_empty_slots_filter*>(filter_.get());
    spin_->setValue(cf->minimum_empty_slots());
}

void min_empty_slots_filter_quick_opt_widget::spin_changed()
{
    if (block_ctl_change_)
        return;
    scoped_value_change<bool> sv(block_filter_change_, true, false);
    minimum_empty_slots_filter* cf = qobject_cast<minimum_empty_slots_filter*>(filter_.get());
    cf->set_minimum_empty_slots(spin_->value());
}
