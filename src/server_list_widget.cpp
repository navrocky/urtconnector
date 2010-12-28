#include <vector>
#include <cassert>

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
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QEvent>

#include <cl/syslog/syslog.h>

#include <common/server_info.h>
#include <common/server_list.h>
#include <geoip/geoip.h>
#include "app_options.h"
#include <filters/filter.h>
#include <filters/filter_edit_widget.h>
#include <filters/filter_list.h>
#include <filters/composite_filter.h>
#include <filters/tools.h>
#include <filters/regexp_filter.h>

#include "server_list_widget.h"
#include "tools.h"

SYSLOG_MODULE(server_list_widget)

const int c_filter_info_column = 100;

//Role to access server_info stored in QTreeModel
// const int c_info_role = Qt::UserRole;
const int c_id_role = Qt::UserRole + 1;
const int c_stamp_role = Qt::UserRole + 2;

//FIXME move to shared place USED in history_tab
const int c_suppress_role = Qt::UserRole + 11;

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

QString server_list_widget_settings::load_toolbar_filter()
{
    part()->beginGroup(name_);
    QString res = part()->value("toolbar_filter_name").toString();
    part()->endGroup();
    return res;
}

void server_list_widget_settings::save_toolbar_filter(const QString& name)
{
    part()->beginGroup(name_);
    part()->setValue("toolbar_filter_name", name);
    part()->endGroup();
}

void server_list_widget_settings::save_state(const QByteArray& a)
{
    part()->beginGroup(name_);
    part()->setValue("state", a);
    part()->endGroup();
}

QByteArray server_list_widget_settings::load_state()
{
    part()->beginGroup(name_);
    QByteArray res = part()->value("state").toByteArray();
    part()->endGroup();
    return res;
}

bool server_list_widget_settings::is_filter_visible()
{
    part()->beginGroup(name_);
    bool res = part()->value("filter_visible").toBool();
    part()->endGroup();
    return res;
}

void server_list_widget_settings::set_filter_visible(bool val)
{
    part()->beginGroup(name_);
    part()->setValue("filter_visible", val);
    part()->endGroup();
}

////////////////////////////////////////////////////////////////////////////////
// server_list_widget
/*
server_list_widget::server_list_widget( filter_factory_p factory, QWidget *parent)
: QMainWindow(parent)
, filters_(new filter_list(factory))
, visible_server_count_(0)
, filter_widget_(0)
{
    setWindowFlags(windowFlags() & (~Qt::Window));

    accum_updater_ = new QAccumulatingConnection(500,
        QAccumulatingConnection::Periodically, this);
    connect(accum_updater_, SIGNAL(signal()), SLOT(update_list()));

    QToolBar* tb = new QToolBar(tr("Filter toolbar"), this);
    tb->setObjectName("filter_toolbar");
    addToolBar(Qt::TopToolBarArea, tb);

    show_filter_action_ = new QAction(QIcon("icons:view-filter.png"), tr("View and edit filter"), this);
    show_filter_action_->setCheckable(true);
    connect(show_filter_action_, SIGNAL(triggered()), SLOT(edit_filter()));
    tb->addAction(show_filter_action_);

    filter_holder_ = new QWidget(this);
    QHBoxLayout* lay = new QHBoxLayout(filter_holder_);
    lay->setContentsMargins(0, 0, 0, 0);
    tb->addWidget(filter_holder_);

    tree_ = new server_tree(this);
    setCentralWidget(tree_);
    
    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->setEditTriggers(QAbstractItemView::EditKeyPressed);
    tree_->setAlternatingRowColors(true);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setRootIsDecorated(false);
    tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);

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

    tree_->setItemDelegateForColumn( 0, new status_item_delegate( serv_list_, this) );
    
    QHeaderView* hdr = tree_->header();
    
    hdr->moveSection(2, 7);
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 50);
    hdr->resizeSection(7, 60);
    hdr->setSortIndicator(4, Qt::AscendingOrder);

    // initialize filters

    // create composite root filter
    filter_p f = filters_->create_by_class_id(composite_filter_class::get_id());
    f->set_name(f->get_class()->caption());
    filters_->set_root_filter(f);
    composite_filter* cf = dynamic_cast<composite_filter*>(f.get());
    
    // create regexp filter as composite child
    f = filters_->create_by_class_id(regexp_filter_class::get_id());
    f->set_name(f->get_class()->caption());
    cf->add_filter(f);

    // select regexp filter for toolbar
    filters_->set_toolbar_filter(f);

    connect(filters_->root_filter().get(), SIGNAL(changed_signal()), SLOT(update_list()));
    connect(filters_.get(), SIGNAL(toolbar_filter_changed()), SLOT(update_toolbar_filter()));
    update_toolbar_filter();
    
    filter_widget_ = new QDockWidget(tr("Filter"), this);
    filter_widget_->setObjectName("filter");
    filter_edit_widget_ = new filter_edit_widget(filters_, filter_widget_);
    filter_widget_->setWidget(filter_edit_widget_);
    filter_widget_->hide();
    addDockWidget(Qt::LeftDockWidgetArea, filter_widget_);
    filter_widget_->installEventFilter(this);
}

server_list_widget::~server_list_widget()
{
}

void server_list_widget::update_toolbar_filter()
{
    // remove old
    foreach (QObject* o, filter_holder_->children())
    {
        if (qobject_cast<QWidget*>(o))
            delete o;
    }

    // create new
    filter_p f = filters_->toolbar_filter().lock();
    if (f)
    {
        QWidget* w = f->get_class()->create_quick_opts_widget(f);
        if (w)
            filter_holder_->layout()->addWidget(w);

        filter_holder_->setToolTip(f->get_class()->caption());
    }
}

void server_list_widget::set_server_list(server_list_p ptr)
{
    if (serv_list_)
        disconnect(serv_list_.get(), SIGNAL(changed()), accum_updater_, SLOT(emitSignal()));
    serv_list_ = ptr;
    if (serv_list_)
    {
        connect(serv_list_.get(), SIGNAL(changed()), accum_updater_, SLOT(emitSignal()));
        tree_->setItemDelegateForColumn( 0, new status_item_delegate( serv_list_, this) );
    }
    accum_updater_->emitSignal();
}

void server_list_widget::set_bookmarks ( server_bookmark_list* bms )
{
    if (bms_)
        disconnect(bms_, SIGNAL(changed()), accum_updater_, SLOT(emitSignal()));
    bms_ = bms;
    if (bms_)
        connect(bms_, SIGNAL(changed()), accum_updater_, SLOT(emitSignal()));
    accum_updater_->emitSignal();
}

void server_list_widget::update_item(QTreeWidgetItem* item)
{
    server_id id = item->data(0, c_id_role).value<server_id>();
    if (id.is_empty())
        return;

    static const server_info_p empty(new server_info);

    server_info_p si = serv_list_->get(id);
    if (!si)
        si = empty;

    int stamp = item->data(0, c_stamp_role).value<int>();
    if (si->update_stamp() != stamp || stamp == 0)
    {
//         QModelIndex index = tree_->indexFromItem(item);
//         tree_->model()->setData(index, QVariant::fromValue(si), c_info_role);

        QString name = si->name;
        if (bms_)
        {
            const server_bookmark& bm = bms_->get(id);
            if (!bm.is_empty())
            {
                if (!bm.name().isEmpty() && name != bm.name())
                {
                    if (name.isEmpty())
                        name = bm.name();
                    else
                        name = QString("%1 (%2)").arg(name).arg(bm.name());
                }
            }
        }

        QStringList sl;
        sl << si->status_name();

        if (si->is_password_needed())
            sl << tr("Private");
        if (si->get_info("pure", "-1").toInt() == 0)
            sl << tr("Not pure");

        QString status = sl.join(", ");

        item->setToolTip(0, status);
        item->setText(1, name);
        item->setText(2, id.address());
        item->setIcon(3, geoip::get_flag_by_country(si->country_code));
        item->setToolTip(3, si->country);
        item->setText(4, QString("%1").arg(si->ping, 5));
        item->setText(5, si->mode_name());
        item->setText(6, si->map);

        QString player_count;
        if (si->max_player_count > 0)
            player_count = QString("%1/%2/%3").arg(si->players.size())
            .arg(si->public_slots()).arg(si->max_player_count);

        item->setText(7, player_count);
        item->setToolTip(7, tr("Current %1 / Public slots %2 / Total %3")
                         .arg(si->players.size()).arg(si->public_slots())
                         .arg(si->max_player_count));
        item->setData(0, c_stamp_role, QVariant::fromValue(si->update_stamp()));
        }

    bool visible = filter_item(item);
    if (visible)
        visible_server_count_++;
    if (item->isHidden() == visible)
        item->setHidden(!visible);
}

bool server_list_widget::filter_item(QTreeWidgetItem* item)
{
    server_id id = item->data(0, c_id_role).value<server_id>();
    server_info_p si = serv_list_->get( id );
    if (filters_->root_filter())
    {
        if (!filters_->root_filter()->filter_server(*(si.get())))
            return false;
    }

    return true;
}

void server_list_widget::force_update()
{
    accum_updater_->emitNow();
}

void server_list_widget::update_list()
{
    LOG_DEBUG << "Update list";
    QTreeWidget* tw = tree_;
    QTreeWidgetItem* cur_item = tw->currentItem();

    visible_server_count_ = 0;
    tw->setUpdatesEnabled(false);
    tw->setSortingEnabled(false);

    const server_info_list& info_list = serv_list_->list();
    server_info_list bm_list;
    const server_info_list* list;

    // take id list
    QList<server_id> ids;
    if (bms_)
    {
        foreach (const server_bookmark& bm, bms_->list())
        {
            server_info_list::const_iterator it = info_list.find(bm.id());
            if (it != info_list.end())
                bm_list[bm.id()] = it->second;
            else
                bm_list[bm.id()] = server_info_p();
        }
        list = &bm_list;
    } else
    {
        list = &info_list;
    }

    // who changed, appeared?
    for (server_info_list::const_iterator it = list->begin(); it != list->end(); it++)
    {
        const server_id& id = it->first;
        server_items::iterator it2 = items_.find(id);

        if (it2 != items_.end())
        {
            update_item(it2->second);
        } else
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(tw);
            item->setData(0, c_id_role, QVariant::fromValue(id));
//             item->setData(0, c_info_role, QVariant::fromValue(it->second));
            items_[id] = item;
            update_item(item);
        }
    }

    // who removed ?
    QList<server_id> to_remove;
    for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
    {
        const server_id& id = it->first;
        if (list->find(id) == list->end())
            to_remove.push_back(id);
    }

    // remove old items
    foreach (const server_id& id, to_remove)
    {
        server_items::iterator it = items_.find(id);
        if (it == items_.end())
            continue;

        QTreeWidgetItem* item = it->second;
        if (item == cur_item)
            cur_item = 0;
        delete item;
        items_.erase(it);
    }

    tw->setSortingEnabled(true);
    tw->setUpdatesEnabled(true);

    if (tw->topLevelItemCount() > 0 && cur_item && app_settings().center_current_row())
        tw->scrollToItem(cur_item, QAbstractItemView::PositionAtCenter);
}

server_id_list server_list_widget::selection()
{
    server_id_list res;
    foreach (QTreeWidgetItem* item, tree_->selectedItems())
    {
        res.push_back(item->data(0, c_id_role).value<server_id>());
    }
    return res;
}

QTreeWidget* server_list_widget::tree() const
{
    return tree_;
}

void server_list_widget::edit_filter()
{
    filter_widget_->setVisible(show_filter_action_->isChecked());
}

void correct_names(filter_list_p fl, filter_p par)
{
    if (par->name().isEmpty())
        par->set_name(fl->correct_name(par->get_class()->caption()));

    composite_filter* cf = dynamic_cast<composite_filter*>(par.get());
    if (!cf)
        return;
    foreach (filter_p f, cf->filters())
        correct_names(fl, f);
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
        correct_names(filters_, filters_->root_filter());

        QString name = st.load_toolbar_filter();
        f = filters_->find_by_name(name);
        filters_->set_toolbar_filter(f);

        restoreState(st.load_state(), 1);
        show_filter_action_->setChecked(st.is_filter_visible());

        filter_edit_widget_->update_contents();
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
    filter_p tbf = filters_->toolbar_filter().lock();
    if (tbf)
        st.save_toolbar_filter(tbf->name());
    else
        st.save_toolbar_filter("");

    st.save_state(saveState(1));
    st.set_filter_visible(show_filter_action_->isChecked());
}

bool server_list_widget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == filter_widget_)
    {
        if (event->type() == QEvent::Show)
            show_filter_action_->setChecked(true);
        if (event->type() == QEvent::Hide)
            show_filter_action_->setChecked(false);
    }
}*/

////////////////////////////////////////////////////////////////////////////////
// status_item_delegate

status_item_delegate::status_item_delegate(server_list_p sl, QObject* parent)
    : QStyledItemDelegate(parent)
    , sl_(sl)
{}

#include <QDir>
#include <iostream>
void status_item_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //Draw base styled-item(gradient backgroud and other)
    QStyledItemDelegate::paint(painter, option, index);

    if( index.data( c_suppress_role ).toBool() )
        return;
    
    //Rect for drawing icons
    const QRect& optr = option.rect;
    QRect icon_rect( optr.x() + 2, optr.y() + 2, optr.height() - 4, optr.height() - 4 );
    
    server_info_p si = ( sl_ )
        ? sl_->get( index.data(c_id_role).value<server_id>() )
        : server_info_p( new server_info() );


    static QPixmap icon_none("icons:status-none.png");
    static QPixmap icon_online("icons:status-online.png");
    static QPixmap icon_offline("icons:status-offline.png");
    static QPixmap icon_updating("icons:status-update.png");
    static QPixmap icon_passwd( "icons:status-passwd.png" );
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

    if ( si->is_password_needed() ) painter->drawPixmap( icon_rect, icon_passwd );

    next_icon( icon_rect);

    if ( si->get_info("pure", "-1").toInt() == 0 ) 
        painter->drawPixmap( icon_rect, QPixmap( "icons:user-identity.png" ) );
}

void status_item_delegate::next_icon(QRect& icon) const
{ 
    icon.adjust( icon.width(), 0, icon.width(), 0 );
}



server_list_tab::server_list_tab(const QString& object_name, filter_factory_p factory, QWidget* parent)
    : main_tab(object_name, parent, factory )
{
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);

    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->setEditTriggers(QAbstractItemView::EditKeyPressed);
    tree_->setAlternatingRowColors(true);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setRootIsDecorated(false);
    tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);

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

    tree_->setItemDelegateForColumn( 0, new status_item_delegate( server_list(), this) );

    QHeaderView* hdr = tree_->header();

    hdr->moveSection(2, 7);
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 50);
    hdr->resizeSection(7, 60);
    hdr->setSortIndicator(4, Qt::AscendingOrder);

    // initialize filters

    init_filter_toolbar();
}

server_list_tab::~server_list_tab()
{}


void server_list_tab::set_bookmarks(server_bookmark_list* bms)
{
    if (bms_)
        disconnect( bms_, SIGNAL( changed() ), this, SLOT( update_servers() ) );
    
    bms_ = bms;
    if (bms_)
        connect( bms_, SIGNAL( changed() ), this, SLOT( update_servers() ) );
    
    update_servers();
}

QTreeWidget* server_list_tab::tree() const
{ return tree_; }


server_id_list server_list_tab::selection() const
{
    server_id_list res;
    foreach ( QTreeWidgetItem* item, tree_->selectedItems() )
        res.push_back( item->data(0, c_id_role).value<server_id>() );

    return res;
}

void server_list_tab::servers_updated()
{
    
    static_cast<status_item_delegate*>(tree_->itemDelegateForColumn(0))->set_server_list( server_list() );
    
    update_list();    
}

void server_list_tab::filter_changed()
{ update_list(); }

void server_list_tab::update_list()
{
    LOG_DEBUG << "Update list";
    QTreeWidgetItem* cur_item = tree_->currentItem();

    visible_server_count_ = 0;
    tree_->setUpdatesEnabled(false);
    tree_->setSortingEnabled(false);

    //full server info list
    const server_info_list& info_list = server_list()->list();

    //bookmark list
    server_info_list bm_list;

    //"current" list - info_list for AllServersTab and bm_list for FavoriteServersTab
    const server_info_list* list;

    // take id list
    QList<server_id> ids;
    if (bms_)
    {
        foreach ( const server_bookmark& bm, bms_->list() )
        {
            server_info_list::const_iterator it = info_list.find( bm.id() );
            if ( it != info_list.end() )
                bm_list[bm.id()] = it->second;
            else
                bm_list[bm.id()] = server_info_p();
        }
        list = &bm_list;
    } else
    {
        list = &info_list;
    }

    // who changed, appeared?
    for (server_info_list::const_iterator it = list->begin(); it != list->end(); it++)
    {
        const server_id& id = it->first; 
        server_items::iterator it2 = items_.find(id);

        if (it2 != items_.end())
        {
            update_item(it2->second);
        }
        else
        {
            QTreeWidgetItem* item = new QTreeWidgetItem( tree_ );
            item->setData( 0, c_id_role, QVariant::fromValue(id) );
            items_[id] = item;
            update_item(item);
        }
    }

    // who removed ?
    QList<server_id> to_remove;
    for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
    {
        const server_id& id = it->first;
        if ( list->find(id) == list->end() )
            to_remove.push_back(id);
    }

    // remove old items
    foreach (const server_id& id, to_remove)
    {
        server_items::iterator it = items_.find(id);
        QTreeWidgetItem* item = it->second;
        if (item == cur_item)
            cur_item = 0;
        delete item;
        items_.erase(it);
    }

    tree_->setSortingEnabled(true);
    tree_->setUpdatesEnabled(true);

    if (tree_->topLevelItemCount() > 0 && cur_item && app_settings().center_current_row())
        tree_->scrollToItem(cur_item, QAbstractItemView::PositionAtCenter);
}

void server_list_tab::update_item(QTreeWidgetItem* item)
{
    server_id id = item->data(0, c_id_role).value<server_id>();
    if (id.is_empty())
        return;

    static const server_info_p empty( new server_info );

    server_info_p si = server_list()->get(id);
    if (!si)
        si = empty;

    int stamp = item->data(0, c_stamp_role).value<int>();
    if ( si->update_stamp() != stamp || stamp == 0 )
    {
        QString name = si->name;
        if (bms_)
        {
            const server_bookmark& bm = bms_->get(id);
            if (!bm.is_empty())
            {
                if (!bm.name().isEmpty() && name != bm.name())
                {
                    if (name.isEmpty())
                        name = bm.name();
                    else
                        name = QString("%1 (%2)").arg(name).arg(bm.name());
                }
            }
        }

        QStringList sl;
        sl << si->status_name();

        if (si->is_password_needed())
            sl << tr("Private");
        if (si->get_info("pure", "-1").toInt() == 0)
            sl << tr("Not pure");

        QString status = sl.join(", ");

        item->setToolTip(0, status);
        item->setText(1, name);
        item->setText(2, id.address());
        item->setIcon(3, geoip::get_flag_by_country(si->country_code));
        item->setToolTip(3, si->country);
        item->setText(4, QString("%1").arg(si->ping, 5));
        item->setText(5, si->mode_name());
        item->setText(6, si->map);

        QString player_count;
        if (si->max_player_count > 0)
            player_count = QString("%1/%2/%3").arg(si->players.size())
            .arg(si->public_slots()).arg(si->max_player_count);

        item->setText(7, player_count);
        item->setToolTip(7, tr("Current %1 / Public slots %2 / Total %3")
                         .arg(si->players.size()).arg(si->public_slots())
                         .arg(si->max_player_count));
        item->setData(0, c_stamp_role, QVariant::fromValue(si->update_stamp()));
        }

    bool visible = filter_item(item);
    if (visible)
        visible_server_count_++;
    if (item->isHidden() == visible)
        item->setHidden(!visible);
}

bool server_list_tab::filter_item(QTreeWidgetItem* item)
{
    server_id id = item->data(0, c_id_role).value<server_id>();
    server_info_p si = server_list()->get( id );
    if ( filterlist().root_filter() )
    {
        if (!filterlist().root_filter()->filter_server(*(si.get())))
            return false;
    }

    return true;
}





