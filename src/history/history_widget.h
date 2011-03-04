#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <common/server_id.h>
#include <tabs/filtered_tab.h>

#include "../pointers.h"

class QAction;
class QTreeWidget;
class QTreeWidgetItem;
class history_item;

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

private:
    void update_item(QTreeWidgetItem*);

    void update_contents_simple();
    void update_contents_grouped();

    void update_caption();
    void update_actions();
    void addItem(const history_item& item);
    QTreeWidgetItem* add_item(QTreeWidgetItem* item);
    QTreeWidgetItem* find_item(const server_id& id) const;
    void resort(QTreeWidgetItem* item);

    QTreeWidget* tree_;
    history_p history_;
    bool group_mode_;
    int item_count_;
    int visible_item_count_;
    bool update_contents_pended_;
    QAction* remove_selected_action_;
    QAction* remove_all_action_;
};

#endif // HISTORY_WIDGET_H
