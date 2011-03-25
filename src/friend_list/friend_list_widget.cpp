#include "friend_list_widget.h"

#include <boost/bind.hpp>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMap>
#include <QAction>

#include <common/tree_smart_updater.h>
#include <tabs/common_item_tags.h>
#include "friend_list.h"
#include "friend_prop_dialog.h"

Q_DECLARE_METATYPE(friend_record)

const int c_friend_role = Qt::UserRole + 12;

friend_list_widget::friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent)
: main_tab(tab_settings_p(new tab_settings("friend_list")), ctx, parent)
, friends_(fl)
, caption_(this, tr("Friends"))
{
    setWindowIcon(QIcon("icons:friends.png"));
    
    add_action_ = new QAction(QIcon("icons:add.png"), tr("Add new friend"), this);
    connect(add_action_, SIGNAL(triggered()), SLOT(add()));

    edit_action_ = new QAction(QIcon("icons:edit.png"), tr("Edit selected friend"), this);
    connect(edit_action_, SIGNAL(triggered()), SLOT(edit_selected()));

    remove_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove selected"), this);
    connect(remove_action_, SIGNAL(triggered()), SLOT(remove_selected()));
    
    addAction(add_action_);
    addAction(edit_action_);
    addAction(remove_action_);
    
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setRootIsDecorated(false);
    tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);
    
    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->addAction(add_action_);
    tree_->addAction(edit_action_);
    tree_->addAction(remove_action_);

    QTreeWidgetItem* it = tree_->headerItem();
    it->setText(7, tr("Address"));
    it->setText(6, tr("Players"));
    it->setText(5, tr("Map"));
    it->setText(4, tr("Game mode"));
    it->setText(3, tr("Ping"));
    it->setText(2, tr("Country"));
    it->setText(1, tr("Name"));
    it->setText(0, tr("Status"));

    QHeaderView* hdr = tree_->header();
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(2, 50);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 60);
    hdr->setSortIndicator(3, Qt::AscendingOrder);
    
    connect(friends_, SIGNAL(changed()), SLOT(update_contents()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(update_actions()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    update_contents();
}

server_id friend_list_widget::selected_server() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
    {
        server_id id = item->data(0, c_id_role).value<server_id>();
        if (id.is_empty() && item->childCount() > 0)
            id = item->child(0)->data(0, c_id_role).value<server_id>();
        return id;
    }
    else
        return server_id();
}

void friend_list_widget::update_contents()
{
    tree_->setRootIsDecorated(false);
    typedef QMap<friend_record, QTreeWidgetItem*> items_map_t;
    const friend_list::friend_records_t& fl = friends_->list();
    updater<friend_list::friend_records_t>::update_tree_contents(fl, c_friend_role, tree_, 0,
        boost::bind(&friend_list_widget::update_friend_item, this, _1), items_map_);

    caption_.set_total_count(tree_->topLevelItemCount());

    update_actions();
}

void friend_list_widget::update_actions()
{
    edit_action_->setEnabled(!(get_selected_friend().is_empty()));
    remove_action_->setEnabled(tree_->selectedItems().count() > 0);
}

void friend_list_widget::update_friend_item(QTreeWidgetItem* item)
{
    const friend_record& fr = item->data(0, c_friend_role).value<friend_record>();
    item->setText(0, fr.nick_name());
}

void friend_list_widget::add()
{
    friend_prop_dialog d(this);
    if (d.exec() == QDialog::Rejected)
        return;
    friends_->add(d.rec());
}

friend_record friend_list_widget::get_selected_friend() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
        return item->data(0, c_friend_role).value<friend_record>();
    else
        return friend_record();
}

void friend_list_widget::edit_selected()
{
    const friend_record& fr = get_selected_friend();
    QString old_nick_name = fr.nick_name();
    friend_prop_dialog d(this);
    d.set_rec(fr);
    if (d.exec() == QDialog::Rejected)
        return;
    friends_->change(old_nick_name, d.rec());
}

void friend_list_widget::remove_selected()
{
    QList<QString> nn;
    for (int i = 0; i < tree_->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* ti = tree_->topLevelItem(i);
        if (ti->isSelected())
        {
            const friend_record& fr = ti->data(0, c_friend_role).value<friend_record>();
            nn.append(fr.nick_name());
        }
    }
    friends_->remove(nn);
}
