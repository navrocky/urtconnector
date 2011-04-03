#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <common/server_id.h>
#include <common/list_caption_updater.h>
#include <common/smart_updater_traits.h>
#include <tabs/filtered_tab.h>

#include "../pointers.h"
#include "history_item.h"

class QAction;
class QTreeWidget;
class QTreeWidgetItem;
class visible_updater;

class history_widget : public filtered_tab
{
    Q_OBJECT
public:
    history_widget(history_p history,
                   const tab_context& ctx,
                   QWidget *parent);

    virtual server_id selected_server() const;

    bool group_mode() const {return group_mode_;}
    void set_group_mode(bool);

public slots:
    void delete_selected();
    void clear_all();

protected slots:
    virtual void filter_changed();

private slots:
    void update_contents();
    void do_selection_change();
    void add_to_favorites();
    void refresh_selected();
    void refresh_all();

private:
    
    typedef updater_traits<history_item>::ItemsByElement items_map_t;
    
    void update_item(QTreeWidgetItem* item, const history_item& hi);
    void update_contents_simple();
    void update_contents_grouped();
    void update_actions();

    QTreeWidget* tree_;
    items_map_t items_map_;
    history_p history_;
    bool group_mode_;
    int item_count_;
    int visible_item_count_;
    QAction* add_bookmark_action_;
    QAction* remove_selected_action_;
    QAction* remove_all_action_;
    QAction* refresh_selected_;
    QAction* refresh_all_;
    list_caption_updater caption_;
    visible_updater* updater_;
};

#endif // HISTORY_WIDGET_H
