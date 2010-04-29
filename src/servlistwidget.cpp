#include <vector>
#include <QTreeWidgetItem>
#include <QtCore/qobject.h>

#include "servlistwidget.h"

const int cFilterInfoColumn = 100;


class ServListItem: public QTreeWidgetItem
{
public:
    ServListItem(QTreeWidget* parent, const server_id& id)
      : QTreeWidgetItem(parent), id_(id) {};
    ServListItem(QTreeWidgetItem* parent, const server_id& id)
      : QTreeWidgetItem(parent), id_(id) {};
    const server_id& id() const {return id_;}
private:
    server_id id_;
};


serv_list_widget::serv_list_widget(QWidget *parent)
    : QWidget(parent),
      servList_(0),
      oldState_(0),
      updateTimer_(0),
      filterTimer_(0)
{
    ui_.setupUi(this);
    updateTimer_ = startTimer(500);
    connect(ui_.filterEdit, SIGNAL(textChanged(const QString&)), SLOT(filterTextChanged(const QString&)));
    connect(ui_.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));
}

serv_list_widget::~serv_list_widget()
{
}

void serv_list_widget::setServerList(serv_list_custom * ptr)
{
    servList_ = ptr;
}

void serv_list_widget::updateItem(ServListItem* item)
{
    const server_info_list_t& list = servList_->list();
    server_info_list_t::const_iterator it = list.find(item->id());
    if (it == list.end()) return;
    const server_info& si = it->second;

    static QIcon icon_none(":/icons/icons/status-none.png");
    static QIcon icon_online(":/icons/icons/status-online.png");
    static QIcon icon_offline(":/icons/icons/status-offline.png");
    static QIcon icon_updating(":/icons/icons/status-update.png");
    static QIcon icon_passwd( ":/icons/icons/status-passwd.png" );

    #if QT_VERSION >= 0x040600
        //trying to load system-wide icon by standard name
        //TODO it is possible to try some standard names such as lock, locked... but later
        icon_passwd = QIcon::fromTheme("object-locked", icon_passwd);
    #endif
    
    switch (si.status)
    {
        case server_info::s_none:
            item->setIcon(0, icon_none);
            break;
        case server_info::s_up:
            if ( si.need_passwd )
                item->setIcon(0, icon_passwd);
            else
                item->setIcon(0, icon_online);
            break;
        case server_info::s_down:
            item->setIcon(0, icon_offline);
            break;
        case server_info::s_updating:
            item->setIcon(0, icon_updating);
            break;
    }

    item->setText(1, si.name.trimmed());
    item->setText(2, si.id.address());
    item->setText(3, QString("%1").arg(si.ping, 5));
    item->setText(4, si.mode_name());
    item->setText(5, si.map);
    item->setText(6, QString("%1/%2").arg(si.players.size()).arg(si.max_player_count));
    item->setText(7, QString::number(si.need_passwd));

    QString players;
    for (player_info_list::const_iterator it = si.players.begin(); it != si.players.end(); it++)
        players += (*it).nick_name + " ";

    item->setText(cFilterInfoColumn, QString("%1 %2 %3 %4 %5 %6").arg(si.name)
        .arg(si.id.address()).arg(si.country).arg(si.map).arg(si.mode_name()).arg(players));
    item->setHidden(!filterItem(item));
}

bool serv_list_widget::filterItem(ServListItem* item)
{
    return filterRx_.isEmpty() ||
            filterRx_.indexIn(item->text(cFilterInfoColumn)) != -1;
}

void serv_list_widget::timerEvent(QTimerEvent *te)
{
    if (!servList_) return;

    if (te->timerId() == updateTimer_)
    {
        if (servList_->state() == oldState_) return;
        oldState_ = servList_->state();
        updateList();
    } else
    if (te->timerId() == filterTimer_)
    {
        updateList();
        killTimer(filterTimer_);
        filterTimer_ = 0;
    }
}

void serv_list_widget::forceUpdate()
{
    oldState_ = servList_->state();
    updateList();
}

void serv_list_widget::updateList()
{
    setUpdatesEnabled(false);
    try
    {
        const server_info_list_t& list = servList_->list();

        // who changed, appeared?
        for (server_info_list_t::const_iterator it = list.begin(); it != list.end(); it++)
        {
            const server_id& id = (*it).first;
            ServItems::iterator it2 = items_.find(id);

            if (it2 != items_.end())
            {
                updateItem((*it2).second);
            } else
            {
                ServListItem* item = new ServListItem(ui_.treeWidget, id);
                items_[id] = item;
                updateItem(item);
            }
        }

        // who removed ?
        std::vector<server_id> to_remove;
        for (ServItems::iterator it = items_.begin(); it != items_.end(); it++)
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
}

void serv_list_widget::filterTextChanged(const QString& val)
{
    filterRx_ = QRegExp(val);
    filterRx_.setCaseSensitivity(Qt::CaseInsensitive);
    if (filterTimer_ != 0)
        killTimer(filterTimer_);
    filterTimer_ = startTimer(500);
}

server_id_list serv_list_widget::selection()
{
    server_id_list res;
    QList<QTreeWidgetItem*> list = tree()->selectedItems();
    for (int i = 0; i < list.size(); i++)
    {
        ServListItem* it = dynamic_cast<ServListItem*>(list[i]);
        if (it)
            res.push_back(it->id());
    }
    return res;
}

void serv_list_widget::filter_clear()
{
    ui_.filterEdit->clear();
}
