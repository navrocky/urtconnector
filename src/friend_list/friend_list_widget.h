#ifndef FRIEND_LIST_WIDGET_H
#define FRIEND_LIST_WIDGET_H

#include <common/server_id.h>
#include <common/list_caption_updater.h>
#include <tabs/main_tab.h>
#include <filters/pointers.h>
#include "friend_record.h"

class QTreeWidgetItem;
class QTreeWidget;
class QAction;
class friend_list;
class server_list;

class friend_list_widget : public main_tab
{
    Q_OBJECT
public:
    friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent);
    
    server_id selected_server() const;


protected:
//    void changeEvent(QEvent *e);
//
//    protected
//Q_SLOTS:
//    virtual void servers_updated();
//    virtual void filter_changed();
//
private slots:
    void update_contents();
    
    void add();
    void edit_selected();
    void remove_selected();

private:
    void update_friend_item(QTreeWidgetItem* item);

    typedef QMap<friend_record, QTreeWidgetItem*> items_map_t;

    QTreeWidget* tree_;
    friend_list* friends_;
    list_caption_updater caption_;
    items_map_t items_map_;
    QAction* add_action_;
    QAction* edit_action_;
    QAction* remove_action_;
};

#endif // FRIEND_LIST_WIDGET_H
