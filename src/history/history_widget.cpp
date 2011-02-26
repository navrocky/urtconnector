#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

#include <common/server_list.h>
#include <tabs/status_item_delegate.h>
#include <tabs/common_item_tags.h>
#include <tabs/status_item_delegate.h>
#include "history.h"

// needed for status_item_delegate
#include "server_list_widget.h"

#include "history_widget.h"

//const int c_id_role = Qt::UserRole + 1;

//FIXME used by status_item_delegate !!
//const int c_suppress_role = Qt::UserRole + 11;

Q_DECLARE_METATYPE(history_item_p);

const int c_history_role = Qt::UserRole + 12;

history_widget::history_widget(history_p history,
                               const tab_context& ctx,
                               QWidget *parent)
: filtered_tab(tab_settings_p(new filtered_tab_settings("history_tab")), ctx, parent)
, history_(history)
{
    setWindowTitle(tr("History"));
    setWindowIcon(QIcon("icons:history.png"));
    
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);
//     connect(p_->ui.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));

    QTreeWidgetItem* it = tree_->headerItem();
    it->setText(0, tr("Server name"));
    it->setText(1, tr("Status"));
    it->setText(2, tr("Date and time"));
    it->setText(3, tr("Address"));
    it->setText(4, tr("Password"));
    it->setText(5, tr("Player name"));

    QHeaderView* hdr = tree_->header();
    hdr->resizeSection(0, 300);
    hdr->resizeSection(1, 80);
    hdr->resizeSection(2, 200);

    tree_->setItemDelegateForColumn(1, new status_item_delegate(server_list(), tree_));
}

//int history_widget::num_rows() const
//{
//    return tree_->topLevelItemCount();
//}

//QTreeWidget* history_widget::tree() const
//{
//    return p_->ui.treeWidget;
//}

void history_widget::update_contents()
{
    tree_->clear();

    foreach (const history_item_p& item, history_->list())
    {
        addItem(item);
    }

    filter_changed();
}

void history_widget::addItem(history_item_p item)
{
    QTreeWidgetItem* item_ptr = new QTreeWidgetItem();

    item_ptr->setText(0, item->server_name());
    item_ptr->setText(2, item->date_time().toString(Qt::DefaultLocaleShortDate));
    item_ptr->setText(3, item->address());
    item_ptr->setText(4, item->password());
    item_ptr->setText(5, item->player_name());
    item_ptr->setData(0, c_id_role, QVariant::fromValue(item->id()));
    item_ptr->setData(1, c_id_role, QVariant::fromValue(item->id()));
    item_ptr->setData(0, c_history_role, QVariant::fromValue(item));

    if (QTreeWidgetItem * parent = add_item(item_ptr))
        resort(parent);
}

QTreeWidgetItem* history_widget::add_item(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parent = find_item(item->data(0, c_id_role).value<server_id > ());

    if (parent)
    {
        item->setData(1, c_suppress_role, true);
        parent->insertChild(0, item);
        return parent;
    }
    else
    {
        item->setData(1, c_suppress_role, false);
        tree_->insertTopLevelItem(0, item);
        return 0;
    }
}

server_id history_widget::selected_server() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
        return item->data(0, c_id_role).value<server_id>();
    else
        return server_id();
}

void history_widget::filter_changed()
{
    for (int i = 0; i < tree_->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tree_->topLevelItem(i);
        bool is_visible = true;
        if (server_list())
        {
            server_info_p si = server_list()->get(item->data(0, c_id_role).value<server_id > ());
            if (si)
                is_visible = filtrate(*si);
        }

        if (item->isHidden() != !is_visible)
            item->setHidden(!is_visible);
    }
}

QTreeWidgetItem* history_widget::find_item(const server_id& id) const
{
    for (int i = 0; i < tree_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = tree_->topLevelItem(i);
        if (item->data(0, c_id_role).value<server_id > () == id)
            return item;
    }
    return 0;
}

void history_widget::resort(QTreeWidgetItem* item)
{
//    int index = tree_->indexOfTopLevelItem(item);
//    if (index == -1)
//        return;
//
//    item = p_->ui.treeWidget->takeTopLevelItem(index);
//
//    QList<QTreeWidgetItem*> chlds = item->takeChildren();
//
//    //item itself is under resorting too!
//    chlds << item;
//
//    std::sort(chlds.begin(), chlds.end(),
//              boost::bind(&QTreeWidgetItem::text, _1, 0) < boost::bind(&QTreeWidgetItem::text, _2, 0));
//
//    std::for_each(chlds.begin(), chlds.end(), boost::bind(&history_widget::add_item, this, _1));
}

void history_widget::delete_selected()
{
//    QList<QTreeWidgetItem*> items = p_->ui.treeWidget->selectedItems();
//
//    int old_size = p_->history->list().size();
//
//    foreach(QTreeWidgetItem* it, items)
//    {
//        history_item_p item = it->data(0, c_history_role).value<history_item_p > ();
//        p_->history->remove(item);
//    }
//
//    if (old_size != p_->history->list().size())
//        update_contents();
}

