#include <QObject>
#include <QComboBox>
#include <QHBoxLayout>

#include <cl/except/error.h>

#include "filter_edit_widget.h"
#include "tools.h"

#include "game_type_filter.h"

////////////////////////////////////////////////////////////////////////////////
// game_type_filter_class

game_type_filter_class::game_type_filter_class()
: filter_class(get_id(), QObject::tr("Game mode filter"),
               QObject::tr("Hides all servers thats don's match selected game mode."))
{}

const char* game_type_filter_class::get_id()
{
    return "game_mode";
}

filter_p game_type_filter_class::create_filter()
{
    return filter_p(new game_type_filter(shared_from_this()));
}

QWidget* game_type_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new game_type_filter_quick_opt_widget(f, parent);
}


////////////////////////////////////////////////////////////////////////////////
// game_type_filter

game_type_filter::game_type_filter(filter_class_p fc)
: filter(fc)
, mode_(server_info::gm_free_for_all)
{
}

bool game_type_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    return si.mode == mode_;
}

void game_type_filter::set_mode(server_info::game_mode mode)
{
    if (mode_ == mode)
        return;
    mode_ = mode;
    emit changed_signal();
}

QByteArray game_type_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << (qint32)mode_;
    return res;
}

void game_type_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    qint32 v;
    ds >> v;
    mode_ = (server_info::game_mode)v;
}


////////////////////////////////////////////////////////////////////////////////
// game_type_filter_quick_opt_widget

void add_item(QComboBox* cb, server_info::game_mode mode)
{
    cb->addItem(server_info::get_mode_name(mode), QVariant::fromValue((int)mode));
}

game_type_filter_quick_opt_widget::game_type_filter_quick_opt_widget(filter_p f, QWidget* parent)
: QWidget(parent)
, filter_(f)
, block_filter_change_(false)
, block_combo_change_(false)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    combo_ = new QComboBox(this);
//    combo_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    l->addWidget(combo_);
    l->addStretch();

    add_item(combo_, server_info::gm_free_for_all);
    add_item(combo_, server_info::gm_team_death_match);
    add_item(combo_, server_info::gm_team_survivor);
    add_item(combo_, server_info::gm_bomb_mode);
    add_item(combo_, server_info::gm_capture_the_flag);
    add_item(combo_, server_info::gm_follow_the_leader);
    add_item(combo_, server_info::gm_capture_and_hold);

    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(combo_, SIGNAL(currentIndexChanged(int)), SLOT(combo_changed()));
}

void game_type_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;
    block_combo_change_ = true;
    game_type_filter* cf = qobject_cast<game_type_filter*>(filter_.get());
    int i = combo_->findData((int)cf->mode());
    combo_->setCurrentIndex(i);
    block_combo_change_ = false;
}

void game_type_filter_quick_opt_widget::combo_changed()
{
    if (block_combo_change_)
        return;
    block_filter_change_ = true;
    game_type_filter* cf = qobject_cast<game_type_filter*>(filter_.get());
    cf->set_mode((server_info::game_mode)(combo_->itemData(combo_->currentIndex())
        .value<int>()));
    block_filter_change_ = false;
}

