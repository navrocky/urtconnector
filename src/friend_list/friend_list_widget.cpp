#include "friend_list_widget.h"

#include <boost/bind.hpp>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMap>
#include <QAction>

#include <common/tree_smart_updater.h>
#include "friend_list.h"
#include "friend_prop_dialog.h"

//#include <common/server_list.h>
//#include "filters/filter_list.h"
//#include "filters/filter.h"
//#include "server_list_widget.h"
//#include "ui_friend_list_widget.h"

//const int c_id_role = Qt::UserRole + 1;

//FIXME used by status_item_delegate !!
//const int c_suppress_role = Qt::UserRole + 11;

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
    update_contents();
}

server_id friend_list_widget::selected_server() const
{
    return server_id();
}

void friend_list_widget::update_contents()
{
    tree_->setRootIsDecorated(false);
    typedef QMap<friend_record, QTreeWidgetItem*> items_map_t;
    const friend_list::friend_records_t& fl = friends_->list();
    updater<friend_list::friend_records_t>::update_tree_contents(fl, c_friend_role, tree_, 0,
        boost::bind(&friend_list_widget::update_friend_item, this, _1), items_map_);
}

void friend_list_widget::update_friend_item(QTreeWidgetItem* item)
{
    
}

void friend_list_widget::add()
{
    friend_prop_dialog d(this);
    if (d.exec() == QDialog::Rejected)
        return;
}

void friend_list_widget::edit_selected()
{
    friend_prop_dialog d(this);
    if (d.exec() == QDialog::Rejected)
        return;

}

void friend_list_widget::remove_selected()
{

}
