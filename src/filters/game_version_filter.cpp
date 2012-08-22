#include "game_version_filter.h"

#include <QObject>
#include <QComboBox>
#include <QHBoxLayout>

#include <cl/except/error.h>

#include "filter_edit_widget.h"
#include "tools.h"

namespace
{
const QString c_version_41("q3ut4");
const QString c_version_42("q3urt42");
}

////////////////////////////////////////////////////////////////////////////////
// game_version_filter_class

game_version_filter_class::game_version_filter_class()
: filter_class(get_id(), QObject::tr("Game version"),
               QObject::tr("Hides all servers thats don's match selected game version."))
{}

const char* game_version_filter_class::get_id()
{
    return "game_version";
}

filter_p game_version_filter_class::create_filter()
{
    return filter_p(new game_version_filter(shared_from_this()));
}

QWidget* game_version_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new game_version_filter_quick_opt_widget(f, parent);
}


////////////////////////////////////////////////////////////////////////////////
// game_version_filter

game_version_filter::game_version_filter(filter_class_p fc)
: filter(fc)
, version_(g_42)
{
}

bool game_version_filter::filter_server(const server_info& si, filter_context& ctx)
{
    if (!enabled())
        return true;

    switch (version_)
    {
        case g_41:
            return si.game_type == c_version_41;
        case g_42:
            return si.game_type == c_version_42;
    }
    return false;
}

void game_version_filter::set_version(game_version_t version)
{
    if (version_ == version)
        return;
    version_ = version;
    emit changed_signal();
}

QByteArray game_version_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    // save own fields
    ds << (qint32)version_;
    return res;
}

void game_version_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");

    qint32 v;
    ds >> v;
    version_ = (game_version_t)v;
}


////////////////////////////////////////////////////////////////////////////////
// game_version_filter_quick_opt_widget

void add_item(QComboBox* cb, const QString& version, game_version_t ver)
{
    cb->addItem(version, QVariant::fromValue((int)ver));
}

game_version_filter_quick_opt_widget::game_version_filter_quick_opt_widget(filter_p f, QWidget* parent)
: QWidget(parent)
, filter_(f)
, block_filter_change_(false)
, block_combo_change_(false)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    combo_ = new QComboBox(this);
    l->addWidget(combo_);
    l->addStretch();

    add_item(combo_, "4.1", g_41);
    add_item(combo_, "4.2", g_42);

    connect(f.get(), SIGNAL(changed_signal()), SLOT(filter_changed()));
    filter_changed();
    connect(combo_, SIGNAL(currentIndexChanged(int)), SLOT(combo_changed()));
}

void game_version_filter_quick_opt_widget::filter_changed()
{
    if (block_filter_change_)
        return;
    block_combo_change_ = true;
    game_version_filter* cf = qobject_cast<game_version_filter*>(filter_.get());
    int i = combo_->findData((int)cf->version());
    combo_->setCurrentIndex(i);
    block_combo_change_ = false;
}

void game_version_filter_quick_opt_widget::combo_changed()
{
    if (block_combo_change_)
        return;
    block_filter_change_ = true;
    game_version_filter* cf = qobject_cast<game_version_filter*>(filter_.get());
    cf->set_version((game_version_t)(combo_->itemData(combo_->currentIndex())
        .value<int>()));
    block_filter_change_ = false;
}

