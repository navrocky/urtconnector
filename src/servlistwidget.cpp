#include <vector>
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
   servList_(0),
   oldState_(0)
{
    ui_.setupUi(this);
    startTimer(500);
}

ServListWidget::~ServListWidget()
{
}

void ServListWidget::setServerList(ServerListCustom * ptr)
{
    servList_ = ptr;
}

void ServListWidget::updateItem(ServListItem* item)
{
    const ServerInfoList& list = servList_->list();
    ServerInfoList::const_iterator it = list.find(item->id());
    if (it == list.end()) return;
    const ServerInfo& si = (*it).second;
    item->setText(1, si.name);
    item->setText(2, si.id.address());
    item->setText(3, QString("%1").arg(si.ping, 5));
    item->setText(4, si.modeName());
    item->setText(5, si.map);
    item->setText(6, QString("%1/%2").arg(si.players.size()).arg(si.maxPlayerCount));
}

void ServListWidget::timerEvent(QTimerEvent *)
{
    if (!servList_) return;
    if (servList_->state() == oldState_) return;
    oldState_ = servList_->state();
    updateList();
}

void ServListWidget::updateList()
{
    setUpdatesEnabled(false);
    try
    {
        const ServerInfoList& list = servList_->list();

        // who changed, appeared?
        for (ServerInfoList::const_iterator it = list.begin(); it != list.end(); it++)
        {
            const ServerID& id = (*it).first;
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
        std::vector<ServerID> to_remove;
        for (ServItems::iterator it = items_.begin(); it != items_.end(); it++)
        {
            const ServerID& id = (*it).first;
            if (list.find(id) == list.end())
                to_remove.push_back(id);
        }
        for (std::vector<ServerID>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
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


