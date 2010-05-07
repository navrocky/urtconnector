
#include <vector>

#include <QTreeWidgetItem>
#include <QtCore/qobject.h>
#include <QPainter>

#include "server_info.h"
#include "server_list.h"
#include "geoip/geoip.h"

#include "server_list_widget.h"

const int c_filter_info_column = 100;
//Role to access server_info stored in QTreeModel
const int c_info_role = Qt::UserRole +1;

class server_list_item: public QTreeWidgetItem
{
public:
    server_list_item(QTreeWidget* parent, const server_id& id)
    : QTreeWidgetItem(parent), id_(id) 
    {};

    server_list_item(QTreeWidgetItem* parent, const server_id& id)
    : QTreeWidgetItem(parent), id_(id) 
    {};

    const server_id& id() const {return id_;}

private:
    server_id id_;
};


server_list_widget::server_list_widget(QWidget *parent)
    : QWidget(parent),
      old_state_(0),
      update_timer_(0),
      filter_timer_(0)
{
    ui_.setupUi(this);
    update_timer_ = startTimer(500);
    connect(ui_.filterEdit, SIGNAL(textChanged(const QString&)), SLOT(filter_text_changed(const QString&)));
    connect(ui_.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));

    ui_.treeWidget->setItemDelegateForColumn( 0, new status_item_delegate(this) );
}

server_list_widget::~server_list_widget()
{
}

void server_list_widget::set_server_list(server_list_p ptr)
{
    serv_list_ = ptr;
}

void server_list_widget::update_item(server_list_item* item)
{
    const server_info_list& list = serv_list_->list();
    server_info_list::const_iterator it = list.find(item->id());
    if (it == list.end()) return;
    server_info_p si = it->second;

//     static QIcon icon_none(":/icons/icons/status-none.png");
//     static QIcon icon_online(":/icons/icons/status-online.png");
//     static QIcon icon_offline(":/icons/icons/status-offline.png");
//     static QIcon icon_updating(":/icons/icons/status-update.png");
//     static QIcon icon_passwd( ":/icons/icons/status-passwd.png" );
// 
//     #if QT_VERSION >= 0x040600
//         //trying to load system-wide icon by standard name
//         //TODO it is possible to try some standard names such as lock, locked... but later
//         icon_passwd = QIcon::fromTheme("object-locked", icon_passwd);
//     #endif

//     switch (si.status)
//     {
//         case server_info::s_none:
//             item->setIcon(0, icon_none);
//             break;
//         case server_info::s_up:
//             item->setIcon(0, icon_online);
//             break;
//         case server_info::s_down:
//             item->setIcon(0, icon_offline);
//             break;
//         case server_info::s_updating:
//             item->setIcon(0, icon_updating);
//             break;
//     }

    QModelIndex index = ui_.treeWidget->indexFromItem(item);
    ui_.treeWidget->model()->setData(index, qVariantFromValue(si), c_info_role );

    item->setText(1, si->name.trimmed());
    item->setText(2, si->id.address());
    item->setIcon(3, geoip::get_flag_by_country(si->country_code) );
    item->setText(3, si->country );
    item->setText(4, QString("%1").arg(si->ping, 5));
    item->setText(5, si->mode_name());
    item->setText(6, si->map);
    item->setText(7, QString("%1/%2").arg(si->players.size()).arg(si->max_player_count));

    QString players;
    for (player_info_list::const_iterator it = si->players.begin(); it != si->players.end(); it++)
        players += (*it).nick_name + " ";

    item->setText(c_filter_info_column, QString("%1 %2 %3 %4 %5 %6").arg(si->name)
        .arg(si->id.address()).arg(si->country).arg(si->map).arg(si->mode_name()).arg(players));
    item->setHidden(!filter_item(item));
}

bool server_list_widget::filter_item(server_list_item* item)
{
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
    setUpdatesEnabled(false);
    try
    {
        const server_info_list& list = serv_list_->list();

        // who changed, appeared?
        for (server_info_list::const_iterator it = list.begin(); it != list.end(); it++)
        {
            const server_id& id = (*it).first;
            server_items::iterator it2 = items_.find(id);

            if (it2 != items_.end())
            {
                update_item((*it2).second);
            } else
            {
                server_list_item* item = new server_list_item(ui_.treeWidget, id);
                items_[id] = item;
                update_item(item);
            }
        }

        // who removed ?
        std::vector<server_id> to_remove;
        for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
        {
            const server_id& id = (*it).first;
            if (list.find(id) == list.end())
                to_remove.push_back(id);
        }
        for (std::vector<server_id>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
        {
            delete items_[*it];
            items_.erase(*it);
        }

        setUpdatesEnabled(true);
    }
    catch(...)
    {
        setUpdatesEnabled(true);
    }
    emit size_changed( items_.size() );
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
    QList<QTreeWidgetItem*> list = tree()->selectedItems();
    for (int i = 0; i < list.size(); i++)
    {
        server_list_item* it = dynamic_cast<server_list_item*>(list[i]);
        if (it)
            res.push_back(it->id());
    }
    return res;
}

void server_list_widget::filter_clear()
{
    ui_.filterEdit->clear();
}


status_item_delegate::status_item_delegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

status_item_delegate::~status_item_delegate()
{}
#include <iostream>
void status_item_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //Draw base styled-item(gradient backgroud and other)
    QStyledItemDelegate::paint(painter, option, index);

    //Rect for drawing icons
    QRect icon( option.rect.topLeft(), QSize(option.rect.height(), option.rect.height()) );
    
    server_info_p si = index.data(c_info_role).value<server_info_p>();

    static QPixmap icon_none(":/icons/icons/status-none.png");
    static QPixmap icon_online(":/icons/icons/status-online.png");
    static QPixmap icon_offline(":/icons/icons/status-offline.png");
    static QPixmap icon_updating(":/icons/icons/status-update.png");
    static QPixmap icon_passwd( ":/icons/icons/status-passwd.png" );
    static QPixmap icon_empty;
    
    
    QPixmap& icon_status = icon_empty;
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
        case server_info::s_updating:
            icon_status = icon_updating;
            break;
    }

    //First icon - status icon
    painter->drawPixmap( icon, icon_status );

    next_icon(icon);

    if ( si->get_info("g_needpass").toInt() ) painter->drawPixmap( icon, icon_passwd );

    next_icon(icon);

    if ( si->get_info("pure", "-1").toInt() == 0 )
        painter->drawPixmap( icon, QPixmap( ":/icons/icons/user-identity.png" ) );
}

void status_item_delegate::next_icon(QRect& icon) const
{ icon.adjust( icon.width(), 0, icon.width(), 0 ); }





