#include <QObject>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTimerEvent>
#include <QAction>

#include <cl/except/error.h>
#include <common/scoped_tools.h>
#include <common/qbuttonlineedit.h>

#include "filter_edit_widget.h"
#include "tools.h"

#include "player_filter.h"

const char* c_nick_name = "nickname";

////////////////////////////////////////////////////////////////////////////////
// player_filter_class

player_filter_class::player_filter_class()
: filter_class(get_id(), QObject::tr("Player nickname"),
               QObject::tr("Finds the server with a specified player."))
{}

const char* player_filter_class::get_id()
{
    return "player_nickname";
}

filter_p player_filter_class::create_filter()
{
    return filter_p(new player_filter(shared_from_this()));
}

QWidget* player_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new player_filter_quick_opt_widget(f, parent);
}


////////////////////////////////////////////////////////////////////////////////
// player_filter

player_filter::player_filter(filter_class_p fc)
: filter(fc)
, use_rx_(false)
, rx_valid_(false)
{
    rx_.setCaseSensitivity(Qt::CaseInsensitive);
}

bool player_filter::filter_server(const server_info& si, filter_context& ctx)
{
    if (!enabled())
        return true;

    if (use_rx_ && !rx_valid_)
    {
        rx_.setPattern(pattern_);
        rx_valid_ = true;
    }

    if (si.players.size() == 0 && pattern().isEmpty())
        return true;

    foreach (const player_info& pi, si.players)
    {
        if (use_rx_)
        {
            if (rx_.isEmpty() || rx_.indexIn(pi.nick_name()) != -1)
            {
                if (ctx.data)
                    ctx.data->insert(c_nick_name, pi.nick_name());
                return true;
            }
        } else
            if (pattern_.isEmpty() || pi.nick_name().contains(pattern_, Qt::CaseInsensitive))
            {
                if (ctx.data)
                    ctx.data->insert(c_nick_name, pi.nick_name());
                return true;
            }
    }
    return false;
}

void player_filter::set_pattern(const QString& val)
{
    if (pattern_ == val)
        return;
    pattern_ = val;
    rx_valid_ = false;
    emit changed_signal();
}

void player_filter::set_use_rx(bool val)
{
    if (use_rx_ == val)
        return;
    use_rx_ = val;
    emit changed_signal();
}

QByteArray player_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << pattern_ << use_rx_;
    return res;
}

void player_filter::load(const QByteArray& ba, filter_factory_p)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    ds >> pattern_ >> use_rx_;
    rx_valid_ = false;
}


////////////////////////////////////////////////////////////////////////////////
// player_filter_quick_opt_widget

player_filter_quick_opt_widget::player_filter_quick_opt_widget(filter_p f, QWidget* parent)
: QWidget(parent)
, filter_(f)
, block_filter_change_(false)
, block_text_change_(false)
, timer_(0)
{
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    edit_ = new QButtonLineEdit(this);
    edit_->setToolTip(tr("Nick name pattern"));
    lay->addWidget(edit_);

    QAction* a = new QAction(QIcon("icons:edit-clear-locationbar-rtl.png"),
                             tr("Clear expression"), this);
    connect(a, SIGNAL(triggered()), edit_, SLOT(clear()));
    edit_->addActionButton(a);

    use_rx_check_ = new QCheckBox(this);
    use_rx_check_->setText(tr("RegExp"));
    use_rx_check_->setToolTip(tr("Pattern used as regular expression"));
    connect(use_rx_check_, SIGNAL(stateChanged(int)), SLOT(use_rx_changed()));
    lay->addWidget(use_rx_check_);
    
    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(edit_, SIGNAL(textChanged(const QString&)), SLOT(text_changed()));
}

void player_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;

    SCOPE_COCK_FLAG(block_text_change_);
    player_filter* rf = qobject_cast<player_filter*>(filter_.get());
    edit_->setText(rf->pattern());
    use_rx_check_->setChecked(rf->use_rx());
}

void player_filter_quick_opt_widget::text_changed()
{
    if (block_text_change_)
        return;

    if (timer_)
        killTimer(timer_);
    timer_ = startTimer(500);
}

void player_filter_quick_opt_widget::use_rx_changed()
{
    if (block_text_change_)
        return;

    SCOPE_COCK_FLAG(block_filter_change_);
    player_filter* cf = qobject_cast<player_filter*>(filter_.get());
    cf->set_use_rx(use_rx_check_->isChecked());
}

void player_filter_quick_opt_widget::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == timer_)
    {
        SCOPE_COCK_FLAG(block_filter_change_);
        player_filter* cf = qobject_cast<player_filter*>(filter_.get());
        cf->set_pattern(edit_->text());

        killTimer(timer_);
        timer_ = 0;

        e->accept();
    }
}

