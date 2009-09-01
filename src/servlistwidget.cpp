#include <QTreeWidgetItem>

#include "servlistwidget.h"

class ServListItem: public QTreeWidgetItem
{
public:
    ServListItem(QTreeWidget* parent, const ServerID& id)
      : QTreeWidgetItem(parent), id_(id) {};
    ServListItem(QTreeWidgetItem* parent, const ServerID& id)
      : QTreeWidgetItem(parent), id_(id) {};
    const ServerID& id() const {return id_;}
private:
    ServerID id_;
};


ServListWidget::ServListWidget(QWidget *parent)
 : QWidget(parent),
   servList_(0)
{
    ui_.setupUi(this);
}


ServListWidget::~ServListWidget()
{
}

void ServListWidget::setServerList(ServerListCustom * ptr)
{
    if (servList_)
    {
        disconnect(servList_);
    }
    servList_ = ptr;
    if (servList_)
    {
        connect(servList_, SIGNAL(serverAdded(const ServerID&)), SLOT(serverAdded(const ServerID&)));
        connect(servList_, SIGNAL(serverChanged(const ServerID&)), SLOT(serverChanged(const ServerID&)));
        connect(servList_, SIGNAL(serverRemoved(const ServerID&)), SLOT(serverRemoved(const ServerID&)));
    }
}

void ServListWidget::serverChanged(const ServerID & id)
{
    ServListItem* item = items_[id];
    updateItem(item);
}

void ServListWidget::serverAdded(const ServerID & id)
{
    ServListItem* item = new ServListItem(ui_.treeWidget, id);
    items_[id] = item;
    updateItem(item);
}

void ServListWidget::serverRemoved(const ServerID & id)
{
    delete items_[id];
    items_.erase(id);
}

void ServListWidget::updateItem(ServListItem* item)
{
    const ServerInfoList& list = servList_->list();
    ServerInfoList::const_iterator it = list.find(item->id());
    if (it == list.end()) return;
    const ServerInfo& si = (*it).second;
    item->setText(1, si.name);
    item->setText(2, si.id.address());
    item->setText(3, QString("%1").arg(si.ping));
    item->setText(4, si.modeName());
    item->setText(5, si.map);
    item->setText(6, QString("%1/%2").arg(si.players.size()).arg(si.maxPlayerCount));
}


