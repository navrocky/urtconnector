#include <QObject>
#include <QHBoxLayout>
#include <QTimerEvent>
#include <QAction>

#include <cl/except/error.h>
#include <common/scoped_tools.h>
#include <common/qbuttonlineedit.h>

#include "filter_edit_widget.h"
#include "tools.h"

#include "regexp_filter.h"

////////////////////////////////////////////////////////////////////////////////
// regexp_filter_class

regexp_filter_class::regexp_filter_class()
: filter_class(get_id(), QObject::tr("Quick filter"),
               QObject::tr("Filters server info using regular expressions."))
{}

const char* regexp_filter_class::get_id()
{
    return "regexp";
}

filter_p regexp_filter_class::create_filter()
{
    return filter_p(new regexp_filter(shared_from_this()));
}

QWidget* regexp_filter_class::create_quick_opts_widget(filter_p f)
{
    return new regexp_filter_quick_opt_widget(f);
}


////////////////////////////////////////////////////////////////////////////////
// regexp_filter

regexp_filter::regexp_filter(filter_class_p fc)
: filter(fc)
{
    rx_.setCaseSensitivity(Qt::CaseInsensitive);
}

bool regexp_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    return rx_.isEmpty() ||
        rx_.indexIn(si.meta_info_string()) != -1;
}

void regexp_filter::set_regexp(const QString& rx)
{
    if (regexp() == rx)
        return;
    rx_.setPattern(rx);
    emit changed_signal();
}

QByteArray regexp_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << rx_.pattern();
    return res;
}

void regexp_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    QString s;
    ds >> s;
    rx_.setPattern(s);
}


////////////////////////////////////////////////////////////////////////////////
// regexp_filter_quick_opt_widget

regexp_filter_quick_opt_widget::regexp_filter_quick_opt_widget(filter_p f)
: filter_(f)
, block_filter_change_(false)
, block_text_change_(false)
, timer_(0)
{
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    edit_ = new QButtonLineEdit(this);
    edit_->setToolTip(tr("Filter expression. You can use here a regular expressions."));
    lay->addWidget(edit_);

    QAction* a = new QAction(QIcon(":/icons/icons/edit-clear-locationbar-rtl.png"),
                             tr("Clear expression"), this);
    connect(a, SIGNAL(triggered()), edit_, SLOT(clear()));
    edit_->addActionButton(a);
    
    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(edit_, SIGNAL(textChanged(const QString&)), SLOT(text_changed()));
}

void regexp_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;

    scoped_value_change<bool> sv(block_text_change_, true, false);
    regexp_filter* rf = qobject_cast<regexp_filter*>(filter_.get());
    edit_->setText(rf->regexp());
}

void regexp_filter_quick_opt_widget::text_changed()
{
    if (block_text_change_)
        return;

    if (timer_)
        killTimer(timer_);
    timer_ = startTimer(500);
}

void regexp_filter_quick_opt_widget::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == timer_)
    {
        scoped_value_change<bool> sv(block_filter_change_, true, false);
        regexp_filter* cf = qobject_cast<regexp_filter*>(filter_.get());
        cf->set_regexp(edit_->text());

        killTimer(timer_);
        timer_ = 0;

        e->accept();
    }
}

