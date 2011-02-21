#ifndef FRIEND_LIST_WIDGET_H
#define FRIEND_LIST_WIDGET_H

//#include <common/server_id.h>
#include <tabs/main_tab.h>
#include <filters/pointers.h>
//#include "pointers.h"

class QTreeWidgetItem;
class QTreeWidget;
class friend_list;
class server_list;

class friend_list_widget : public main_tab
{
    Q_OBJECT
public:
    friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent);
//    QTreeWidget* tree() const;
//    void update_history();
//    int num_rows() const;

//    virtual server_id selected_server() const;


//    public
//Q_SLOTS:
//    void delete_selected();


protected:
//    void changeEvent(QEvent *e);
//
//    protected
//Q_SLOTS:
//    virtual void servers_updated();
//    virtual void filter_changed();
//
private slots:
//    void filter_clear();

private:
    void update_list();

//    void addItem(history_item_p item);
    ///returns 0 if NO resort needed
//    QTreeWidgetItem* add_item(QTreeWidgetItem* item);
//    QTreeWidgetItem* find_item(const server_id& id) const;

//    void resort(QTreeWidgetItem* item);

    QTreeWidget* tree_;
    friend_list* friends_;
};

#endif // FRIEND_LIST_WIDGET_H
