#include "timer_condition.h"

#include <QObject>
#include <QTimer>
#include <QFormLayout>
#include <QSpinBox>

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// timer_condition_class

timer_condition_class::timer_condition_class(const context_p& ctx)
: condition_class(ctx, "timer", 
                  QObject::tr("Timer"),
                  QObject::tr("Actions activates by timer"),
                  QIcon("icons:chronometer.png"))
{
}

condition_p timer_condition_class::create()
{
    return condition_p(new timer_condition(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// timer_condition

timer_condition::timer_condition(const condition_class_p& c)
: condition_t(c)
{
    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    connect(timer_, SIGNAL(timeout()), SLOT(trigger()));
}

int timer_condition::interval() const
{
    return timer_->interval();
}

void timer_condition::set_interval(int val)
{
    if (val == interval())
        return;
    timer_->setInterval(val);
    changed();
}

void timer_condition::save(settings_t& s)
{
    s["interval"] = interval();
}

void timer_condition::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("interval");
    if (it != s.end())
        set_interval(it.value().toInt());
}

void timer_condition::do_start()
{
    timer_->start();
}

void timer_condition::do_stop()
{
    timer_->stop();
}

QWidget* timer_condition::create_options_widget(QWidget* parent)
{
    return new timer_condition_widget(parent, this);
}

void timer_condition::skip_current()
{
    // nothing to do
}

////////////////////////////////////////////////////////////////////////////////
// timer_condition_widget

timer_condition_widget::timer_condition_widget(QWidget* parent, timer_condition* cond)
: QWidget(parent)
, cond_(cond)
{
    QFormLayout* l = new QFormLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    spin_ = new QSpinBox(this);
    spin_->setMinimum(100);
    spin_->setMaximum(100000);
    spin_->setSuffix(tr(" msec"));
    spin_->setSingleStep(100);
    l->addRow(tr("Interval"), spin_);
    spin_->setValue(cond->interval());
    connect(spin_, SIGNAL(valueChanged(int)), SLOT(spin_changed(int)));
}

void timer_condition_widget::spin_changed(int val)
{
    cond_->set_interval(val);
}

}
