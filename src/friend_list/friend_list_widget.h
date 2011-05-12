#ifndef FRIEND_LIST_WIDGET_H
#define FRIEND_LIST_WIDGET_H

#include <set>
#include <common/server_id.h>
#include <common/list_caption_updater.h>
#include <common/smart_updater_traits.h>
#include <tabs/main_tab.h>
#include <filters/pointers.h>
#include "friend_record.h"

class QTreeWidgetItem;
class QTreeWidget;
class QAction;
class friend_list;
class server_list;
class visible_updater;
class name_delegate;
class status_item_delegate;

class friend_list_widget : public main_tab
{
    Q_OBJECT
public:
    friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent);
    
    server_id selected_server() const;

public slots:
    virtual void save_state();
    virtual void load_state();

private slots:
    void update_contents();
    void update_actions();
    
    void add();
    void edit_selected();
    void remove_selected();

    void update_selected();
    void update_bookmarks();
    void update_all();

private:
    void update_friend_item(QTreeWidgetItem* item, const friend_record& fr );
    void update_server_item(QTreeWidgetItem* item, const server_id& id );
    server_id_list find_server_with_player(const friend_record& fr);

    friend_record get_selected_friend() const;

    typedef std::set<server_id> server_set_t;
    server_set_t get_selected_servers() const;

    typedef updater_traits<friend_record>::ItemsByElement items_map_t;

    QTreeWidget* tree_;
    friend_list* friends_;
    list_caption_updater caption_;
    items_map_t items_map_;
    visible_updater* updater_;
    QAction* add_action_;
    QAction* edit_action_;
    QAction* remove_action_;
    QAction* update_selected_action_;
    QAction* update_bookmarks_action_;
    QAction* update_all_action_;
    int online_count_;
    name_delegate* name_delegate_;
    status_item_delegate* status_delegate_;
};

//class friend_list_widget_settings
//{
//public:
//    friend_list_widget_settings(const tab_settings_p& ts);
//
//    QByteArray header_state() const;
//    void save_header_state(const QByteArray&);
//
//private:
//    QString uid_;
//    base_settings::settings_ptr sls;
//};

#endif // FRIEND_LIST_WIDGET_H
