#include <QTreeWidget>
#include <QHeaderView>

//#include <common/server_list.h>
//#include "filters/filter_list.h"
//#include "filters/filter.h"
//#include "server_list_widget.h"
#include "friend_list_widget.h"
//#include "ui_friend_list_widget.h"

//const int c_id_role = Qt::UserRole + 1;

//FIXME used by status_item_delegate !!
//const int c_suppress_role = Qt::UserRole + 11;

//Q_DECLARE_METATYPE(history_item_p);

//const int c_history_role = Qt::UserRole + 12;


friend_list_widget::friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent)
: main_tab("friend_list_widget", ctx, parent)
, friends_(fl)
{
//    init_filter_toolbar();

    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);

    QHeaderView* hdr = tree_->header();

    hdr->resizeSection(0, 300);
    hdr->resizeSection(1, 80);
    hdr->resizeSection(2, 200);






    //     connect(p_->ui.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));
}

void friend_list_widget::update_list()
{
    

}

//void friend_list_widget::changeEvent(QEvent *e)
//{
//    QWidget::changeEvent(e);
//    switch (e->type())
//    {
//        case QEvent::LanguageChange:
//            p_->retranslate();
//            break;
//        default:
//            break;
//    }
//}

//void friend_list_widget::filter_clear()
//{
//    //     p_->ui.filterEdit->clear();
//}

//QTreeWidget* friend_list_widget::tree() const
//{
//    return p_->ui.treeWidget;
//}

//void friend_list_widget::update_history()
//{
//    p_->ui.treeWidget->clear();
//
//    foreach(const history_item_p& item, p_->history->list())
//    {
//        addItem(item);
//    }
//    filter_changed();
//}

//void friend_list_widget::addItem(history_item_p item)
//{
//    QTreeWidgetItem* item_ptr = new QTreeWidgetItem();
//
//    item_ptr->setText(0, item->server_name());
//    item_ptr->setText(2, item->date_time().toString(Qt::DefaultLocaleShortDate));
//    item_ptr->setText(3, item->address());
//    item_ptr->setText(4, item->password());
//    item_ptr->setText(5, item->player_name());
//    item_ptr->setData(0, c_id_role, QVariant::fromValue(item->id()));
//    item_ptr->setData(1, c_id_role, QVariant::fromValue(item->id()));
//
//    item_ptr->setData(0, c_history_role, QVariant::fromValue(item));
//
//    if (QTreeWidgetItem * parent = add_item(item_ptr))
//        resort(parent);
//}

//QTreeWidgetItem* friend_list_widget::add_item(QTreeWidgetItem* item)
//{
//    QTreeWidgetItem* parent = find_item(item->data(0, c_id_role).value<server_id > ());
//
//    if (parent)
//    {
//        item->setData(1, c_suppress_role, true);
//        parent->insertChild(0, item);
//        return parent;
//    }
//    else
//    {
//        item->setData(1, c_suppress_role, false);
//        p_->ui.treeWidget->insertTopLevelItem(0, item);
//        return 0;
//    }
//}

//server_id friend_list_widget::selected_server() const
//{
//    QTreeWidgetItem* item = p_->ui.treeWidget->currentItem();
//    if (!item)
//        return server_id();
//
//    return item->data(0, c_id_role).value<server_id > ();
//}

//void friend_list_widget::servers_updated()
//{
//    p_->ui.treeWidget->setItemDelegateForColumn(1, new status_item_delegate(server_list(), p_->ui.treeWidget));
//}

//void friend_list_widget::filter_changed()
//{
//    QList<QTreeWidgetItem *> items = p_->ui.treeWidget->findItems("", Qt::MatchStartsWith);
//
//    foreach(QTreeWidgetItem * item, items)
//    {
//        bool is_visible = true;
//        if (server_list())
//        {
//            server_info_p si = server_list()->get(item->data(0, c_id_role).value<server_id > ());
//            if (si)
//                is_visible = filterlist().filtrate(*si);
//        }
//
//        if (item->isHidden() != !is_visible)
//            item->setHidden(!is_visible);
//    }
//}

//QTreeWidgetItem* friend_list_widget::find_item(const server_id& id) const
//{
//    for (int i = 0; i < p_->ui.treeWidget->topLevelItemCount(); ++i)
//    {
//        QTreeWidgetItem* item = p_->ui.treeWidget->topLevelItem(i);
//        if (item->data(0, c_id_role).value<server_id > () == id)
//            return item;
//    }
//
//    return 0;
//}

//void friend_list_widget::resort(QTreeWidgetItem* item)
//{
//    int index = p_->ui.treeWidget->indexOfTopLevelItem(item);
//
//    if (index == -1) return;
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
//    std::for_each(chlds.begin(), chlds.end(), boost::bind(&friend_list_widget::add_item, this, _1));
//}

//void friend_list_widget::delete_selected()
//{
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
//        update_history();
//}

