#include <vector>

#include <QTreeWidgetItem>
#include <QPainter>
#include <QStringList>
#include <QHeaderView>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QPixmap>
#include <QTimerEvent>
#include <QDialog>
#include <QSettings>

#include <cl/syslog/syslog.h>

#include "server_info.h"
#include "server_list.h"
#include "geoip/geoip.h"
#include "app_options.h"
#include "filters/filter.h"
#include "filters/filter_edit_widget.h"
#include "filters/filter_list.h"
#include "filters/composite_filter.h"
#include "filters/tools.h"

#include "server_list_widget.h"

SYSLOG_MODULE("server_list_widget")

const int c_filter_info_column = 100;

//Role to access server_info stored in QTreeModel
const int c_info_role = Qt::UserRole;

////////////////////////////////////////////////////////////////////////////////
// server_tree

server_tree::server_tree(QWidget* p)
    :QTreeWidget(p)
{}

QModelIndex server_tree::indexFromItem(QTreeWidgetItem* item) const
{
    return QTreeWidget::indexFromItem(item);
}

////////////////////////////////////////////////////////////////////////////////
// server_tree

server_list_widget_settings::server_list_widget_settings(const QString& list_name)
: name_(list_name)
{
}

filter_p server_list_widget_settings::load_root_filter(filter_factory_p factory)
{
    part()->beginGroup(name_);
    QByteArray ba = part()->value("root_filter").toByteArray();
    part()->endGroup();
    return filter_load(ba, factory);
}

void server_list_widget_settings::save_root_filter(filter_p f)
{
    part()->beginGroup(name_);
    part()->setValue("root_filter", filter_save(f));
    part()->endGroup();
}


////////////////////////////////////////////////////////////////////////////////
// server_list_widget

server_list_widget::server_list_widget(app_options_p opts,  filter_factory_p factory,
    QWidget *parent)
: QWidget(parent)
, old_state_(0)
, update_timer_(0)
, filter_timer_(0)
, favs_(0)
, opts_(opts)
, filters_(new filter_list(factory))
{
    QBoxLayout* vert_lay = new QVBoxLayout(this);
    vert_lay->setContentsMargins(0, 0, 0, 0);
    QBoxLayout* horiz_lay = new QHBoxLayout();

    show_filter_button_ = new QToolButton(this);
    show_filter_button_->setIcon(QIcon(":/icons/icons/view-filter.png"));
    show_filter_button_->setAutoRaise(true);
    show_filter_button_->setToolTip(tr("View and edit filter"));

    connect(show_filter_button_, SIGNAL(clicked()), SLOT(edit_filter()));

    horiz_lay->addWidget(show_filter_button_);

    filter_edit_ = new QLineEdit(this);

    horiz_lay->addWidget(filter_edit_);

    clear_filter_button_ = new QToolButton(this);
    clear_filter_button_->setIcon(QIcon(":/icons/icons/edit-clear-locationbar-rtl.png"));
    clear_filter_button_->setToolButtonStyle(Qt::ToolButtonIconOnly);
    clear_filter_button_->setAutoRaise(true);

    horiz_lay->addWidget(clear_filter_button_);

    vert_lay->addLayout(horiz_lay);

    tree_ = new server_tree(this);
    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->setEditTriggers(QAbstractItemView::EditKeyPressed);
    tree_->setAlternatingRowColors(true);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setRootIsDecorated(false);
    tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);

    vert_lay->addWidget(tree_);

    clear_filter_button_->setText(tr("Clear filter"));
    clear_filter_button_->setToolTip(tr("Clear current filter"));

    filter_edit_->setToolTip(tr("Filter line. You can use here a regular expressions."));

    QTreeWidgetItem *hi = tree_->headerItem();
    hi->setText(7, tr("Players"));
    hi->setText(6, tr("Map"));
    hi->setText(5, tr("Game mode"));
    hi->setText(4, tr("Ping"));
    hi->setText(3, tr("Country"));
    hi->setText(2, tr("Address"));
    hi->setText(1, tr("Name"));
    hi->setText(0, tr("Status"));
    hi->setToolTip(2, tr("Server address (ip:port)"));
    hi->setToolTip(1, tr("Server name"));
    hi->setToolTip(0, tr("Server status"));

    update_timer_ = startTimer(500);
    connect(filter_edit_, SIGNAL(textChanged(const QString&)), SLOT(filter_text_changed(const QString&)));
    connect(clear_filter_button_, SIGNAL(clicked()), SLOT(filter_clear()));

    tree_->setItemDelegateForColumn( 0, new status_item_delegate(this) );
    
    QHeaderView* hdr = tree_->header();
    
    hdr->moveSection(2, 7);
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 50);
    hdr->resizeSection(7, 60);
    hdr->setSortIndicator(4, Qt::AscendingOrder);

    filter_p f = filters_->create_by_class_id(composite_filter_class::get_id());
    filters_->set_root_filter(f);
    connect(f.get(), SIGNAL(changed_signal()), SLOT(update_list()));
}

server_list_widget::~server_list_widget()
{
    delete edit_widget_;
}

void server_list_widget::set_server_list(server_list_p ptr)
{
    serv_list_ = ptr;
}

void server_list_widget::set_favs ( server_fav_list* favs )
{
    favs_ = favs;
}

void server_list_widget::update_item(QTreeWidgetItem* item)
{
    server_info_p si = item->data(0, c_info_role).value<server_info_p>();
    if (!si)
        return;

    QModelIndex index = tree_->indexFromItem(item);
    tree_->model()->setData(index, qVariantFromValue(si), c_info_role );

    QString name = si->name;
    if (favs_)
    {
        server_fav_list::iterator it = favs_->find(si->id);
        if (it != favs_->end())
        {
            QString fav_name = it->second.name;
            if (!fav_name.isEmpty() && name != fav_name)
            {
                if (name.isEmpty())
                    name = fav_name;
                else
                    name = QString("%1 (%2)").arg(name).arg(fav_name);
            }
        }
    }

    QStringList sl;
    sl << si->status_name();

    if ( si->get_info("g_needpass").toInt() )
        sl << tr("Private");
    if ( si->get_info("pure", "-1").toInt() == 0 )
        sl << tr("Not pure");

    QString status = sl.join(", ");

    int private_slots = si->get_info("sv_privateClients").toInt();

    item->setToolTip(0, status);
    item->setText(1, name);
    item->setText(2, si->id.address());
    item->setIcon(3, geoip::get_flag_by_country(si->country_code) );
    item->setToolTip(3, si->country);
    item->setText(4, QString("%1").arg(si->ping, 5));
    item->setText(5, si->mode_name());
    item->setText(6, si->map);

    item->setText(7, QString("%1/%2/%3").arg(si->players.size())
        .arg(si->max_player_count - private_slots).arg(si->max_player_count));
    item->setToolTip(7, tr("Current %1 / Public slots %2 / Total %3")
        .arg(si->players.size()).arg(si->max_player_count - private_slots).arg(si->max_player_count));

    QString players;
    for (player_info_list::const_iterator it = si->players.begin(); it != si->players.end(); it++)
        players += (*it).nick_name + " ";

    item->setText(c_filter_info_column, QString("%1 %2 %3 %4 %5 %6 %7 %8").arg(name)
        .arg(si->id.address()).arg(si->country).arg(si->map).arg(si->mode_name()).arg(players)
        .arg(si->country).arg(status));
    item->setHidden(!filter_item(item));
}

bool server_list_widget::filter_item(QTreeWidgetItem* item)
{
    server_info_p si = item->data(0, c_info_role).value<server_info_p>();
    if (filters_->root_filter())
    {
        if (!filters_->root_filter()->filter_server(*(si.get())))
            return false;
    }

    return filter_rx_.isEmpty() ||
            filter_rx_.indexIn(item->text(c_filter_info_column)) != -1;
}

void server_list_widget::timerEvent(QTimerEvent *te)
{
    if (!serv_list_) return;

    if (te->timerId() == update_timer_)
    {
        if (serv_list_->state() == old_state_) return;
        old_state_ = serv_list_->state();
        update_list();
    } else
    if (te->timerId() == filter_timer_)
    {
        update_list();
        killTimer(filter_timer_);
        filter_timer_ = 0;
    }
}

void server_list_widget::force_update()
{
    old_state_ = serv_list_->state();
    update_list();
}

void server_list_widget::update_list()
{
    LOG_DEBUG << "update_list()";
    QTreeWidget* tw = tree_;
    QTreeWidgetItem* cur_item = tw->currentItem();

    tw->setUpdatesEnabled(false);
    try
    {
        const server_info_list& list = serv_list_->list();

        // who changed, appeared?
        for (server_info_list::const_iterator it = list.begin(); it != list.end(); it++)
        {
            const server_id& id = it->first;
            server_items::iterator it2 = items_.find(id);

            if (it2 != items_.end())
            {
                update_item(it2->second);
            } else
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(tw);
                item->setData(0, c_info_role, QVariant::fromValue(it->second));
                items_[id] = item;
                update_item(item);
            }
        }
        // who removed ?
        std::vector<server_id> to_remove;
        for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
        {
            const server_id& id = it->first;
            if (list.find(id) == list.end())
                to_remove.push_back(id);
        }
        for (std::vector<server_id>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
        {
            QTreeWidgetItem* item = items_[*it];
            if (item == cur_item)
                cur_item == 0;
            delete item;
            items_.erase(*it);
        }
        tw->setUpdatesEnabled(true);
    }
    catch(...)
    {
        tw->setUpdatesEnabled(true);
    }

    if (tw->topLevelItemCount() > 0 && cur_item && opts_->center_current_row)
        tw->scrollToItem(cur_item, QAbstractItemView::PositionAtCenter);
}

void server_list_widget::filter_text_changed(const QString& val)
{
    filter_rx_ = QRegExp(val);
    filter_rx_.setCaseSensitivity(Qt::CaseInsensitive);
    if (filter_timer_ != 0)
        killTimer(filter_timer_);
    filter_timer_ = startTimer(500);
}

server_id_list server_list_widget::selection()
{
    server_id_list res;
    foreach (QTreeWidgetItem* item, tree_->selectedItems())
    {
        server_info_p si = item->data(0, c_info_role).value<server_info_p>();
        if (si)
            res.push_back(si->id);
    }
    return res;
}

void server_list_widget::filter_clear()
{
    filter_edit_->clear();
}

QTreeWidget* server_list_widget::tree() const
{
    return tree_;
}

void server_list_widget::edit_filter()
{
    if (!edit_widget_)
    {
        edit_widget_ = new filter_edit_widget(filters_);
//        edit_widget_->setAttribute(Qt::WA_DeleteOnClose, true);
    }
    edit_widget_->move(show_filter_button_->mapToGlobal(QPoint(0, show_filter_button_->height())));
    edit_widget_->show();
}

void server_list_widget::load_options()
{
    assert(!objectName().isEmpty());
    try
    {
        server_list_widget_settings st(objectName());
        filter_p f = st.load_root_filter(filters_->factory());
        filters_->set_root_filter(f);
        connect(f.get(), SIGNAL(changed_signal()), SLOT(update_list()));
    }
    catch(const std::exception& e)
    {
        LOG_ERR << e.what();
    }
}

void server_list_widget::save_options()
{
    assert(!objectName().isEmpty());
    server_list_widget_settings st(objectName());
    st.save_root_filter(filters_->root_filter());
}

////////////////////////////////////////////////////////////////////////////////
// status_item_delegate

status_item_delegate::status_item_delegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void status_item_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //Draw base styled-item(gradient backgroud and other)
    QStyledItemDelegate::paint(painter, option, index);

    //Rect for drawing icons
    const QRect& optr = option.rect;
    QRect icon_rect( optr.x() + 2, optr.y() + 2, optr.height() - 4, optr.height() - 4 );
    
    server_info_p si = index.data(c_info_role).value<server_info_p>();
    if ( !si ) si = server_info_p( new server_info() );

    static QPixmap icon_none(":/icons/icons/status-none.png");
    static QPixmap icon_online(":/icons/icons/status-online.png");
    static QPixmap icon_offline(":/icons/icons/status-offline.png");
    static QPixmap icon_updating(":/icons/icons/status-update.png");
    static QPixmap icon_passwd( ":/icons/icons/status-passwd.png" );
    static QPixmap icon_empty;
    
    QPixmap& icon_status = icon_empty;

    if (si->updating)
        icon_status = icon_updating;
    else
        switch (si->status)
        {
            case server_info::s_none:
                icon_status = icon_none;
                break;
            case server_info::s_up:
                icon_status = icon_online;
                break;
            case server_info::s_down:
                icon_status = icon_offline;
                break;
        }

    //First icon - status icon
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap( icon_rect, icon_status );

    next_icon( icon_rect);

    if ( si->get_info("g_needpass").toInt() ) painter->drawPixmap( icon_rect, icon_passwd );

    next_icon( icon_rect);

    if ( si->get_info("pure", "-1").toInt() == 0 )
        painter->drawPixmap( icon_rect, QPixmap( ":/icons/icons/user-identity.png" ) );
}

void status_item_delegate::next_icon(QRect& icon) const
{ 
    icon.adjust( icon.width(), 0, icon.width(), 0 );
}


