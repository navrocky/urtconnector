#include <set>
#include <boost/make_shared.hpp>

#include <QObject>
#include <QSpinBox>
#include <QHBoxLayout>

#include <cl/except/error.h>
#include <common/scoped_tools.h>

#include "filter_edit_widget.h"
#include "tools.h"
#include "../tools.h"

#include "online_clans_filter.h"

////////////////////////////////////////////////////////////////////////////////
// online_clans_filter_class

online_clans_filter_class::online_clans_filter_class()
: filter_class(get_id(), QObject::tr("Online clans"),
    QObject::tr("Shows servers on which present a some number of players with the same tag."))
{}

const char* online_clans_filter_class::get_id()
{
    return "online_clans";
}

filter_p online_clans_filter_class::create_filter()
{
    return boost::make_shared<online_clans_filter>(shared_from_this());
}

QWidget* online_clans_filter_class::create_quick_opts_widget(filter_p f)
{
    return new online_clans_filter_quick_opt_widget(f);
}


////////////////////////////////////////////////////////////////////////////////
// online_clans_filter

online_clans_filter::online_clans_filter(filter_class_p fc)
: filter(fc)
, minimal_tag_length_(3)
, minimal_players_(3)
{
}

bool online_clans_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    typedef std::set<QString> sset_t;
    sset_t m;

    foreach (const player_info& p1, si.players)
    {
        foreach (const player_info& p2, si.players)
        {
            if (p1.nick_name == p2.nick_name)
                continue;
            QString s = common_substring_from_begin(p1.nick_name, p2.nick_name);
            if (s.length() >= minimal_tag_length_)
                m.insert(s);
        }
    }

    foreach (sset_t::const_reference r, m)
    {
        int cnt = 0;
        foreach (const player_info& p, si.players)
        {
            if (p.nick_name.startsWith(r))
                cnt++;
        }
        if (cnt >= minimal_players_)
            return true;
    }

    return false;
}

void online_clans_filter::set_minimal_tag_length(int val)
{
    if (minimal_tag_length_ == val)
        return;
    minimal_tag_length_ = val;
    emit changed_signal();
}

void online_clans_filter::set_minimal_players(int val)
{
    if (minimal_players_ == val)
        return;
    minimal_players_ = val;
    emit changed_signal();
}

QByteArray online_clans_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << (qint32)minimal_tag_length_;
    ds << (qint32)minimal_players_;
    return res;
}

void online_clans_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    qint32 v;
    ds >> v;
    minimal_tag_length_ = v;
    ds >> v;
    minimal_players_ = v;
}


////////////////////////////////////////////////////////////////////////////////
// online_clans_filter_quick_opt_widget

online_clans_filter_quick_opt_widget::online_clans_filter_quick_opt_widget(filter_p f)
: filter_(f)
, block_filter_change_(false)
, block_ctl_change_(false)
{
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    tag_length_spin_ = new QSpinBox(this);
    tag_length_spin_->setToolTip(tr("Minimal tag length in chars"));
    lay->addWidget(tag_length_spin_);

    players_spin_ = new QSpinBox(this);
    players_spin_->setToolTip(tr("Minimal players number"));
    lay->addWidget(players_spin_);
    
    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(tag_length_spin_, SIGNAL(valueChanged(int)), SLOT(tag_length_changed()));
    connect(players_spin_, SIGNAL(valueChanged(int)), SLOT(players_changed()));

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void online_clans_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;
    scoped_value_change<bool> sv(block_ctl_change_, true, false);
    online_clans_filter* cf = qobject_cast<online_clans_filter*>(filter_.get());
    tag_length_spin_->setValue(cf->minimal_tag_length());
    players_spin_->setValue(cf->minimal_players());
}

void online_clans_filter_quick_opt_widget::tag_length_changed()
{
    if (block_ctl_change_)
        return;
    scoped_value_change<bool> sv(block_filter_change_, true, false);
    online_clans_filter* cf = qobject_cast<online_clans_filter*>(filter_.get());
    cf->set_minimal_tag_length(tag_length_spin_->value());
}

void online_clans_filter_quick_opt_widget::players_changed()
{
    if (block_ctl_change_)
        return;
    scoped_value_change<bool> sv(block_filter_change_, true, false);
    online_clans_filter* cf = qobject_cast<online_clans_filter*>(filter_.get());
    cf->set_minimal_players(players_spin_->value());
}

