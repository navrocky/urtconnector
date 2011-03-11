#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <common/server_id.h>
#include <tabs/filtered_tab.h>
#include <common/list_caption_updater.h>

#include "../pointers.h"
#include "history_item.h"

class QAction;
class QTreeWidget;
class QTreeWidgetItem;

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

protected:
    void showEvent(QShowEvent* event);

private slots:
    void update_contents();
    void do_selection_change();
    void add_to_favorites();

private:
    typedef QMap<history_item, QTreeWidgetItem*> items_map_t;
    
    void update_item(QTreeWidgetItem*);
    void update_contents_simple();
    void update_contents_grouped();
    void update_actions();

    QTreeWidget* tree_;
    items_map_t items_map_;
    history_p history_;
    bool group_mode_;
    int item_count_;
    int visible_item_count_;
    bool update_contents_pended_;
    QAction* add_bookmark_action_;
    QAction* remove_selected_action_;
    QAction* remove_all_action_;
    list_caption_updater caption_;
};

#endif // HISTORY_WIDGET_H
