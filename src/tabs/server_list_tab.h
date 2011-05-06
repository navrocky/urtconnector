#ifndef SERVER_LIST_TAB_H
#define	SERVER_LIST_TAB_H

#include <QPointer>

#include "server_list_common_tab.h"
#include <common/smart_updater_traits.h>

class QTreeWidget;
class QTreeWidgetItem;
class QAction;
class server_bookmark_list;
class visible_updater;

class server_list_tab : public server_list_common_tab
{
    Q_OBJECT
public:
    server_list_tab(const QString& object_name,
                    const tab_context& ctx,
                    QWidget* parent);

protected slots:
    virtual void do_selection_change();

private slots:
    void update_contents();
    void add_to_favorites();
    void refresh_all();
    void clear_all();

private:
    void update_actions();
    void update_item(QTreeWidgetItem* item, const server_id& id );

    typedef updater_traits<server_id>::ItemsByElement server_items;
    server_items items_;

    QAction* add_bookmark_action_;
    QAction* refresh_selected_action_;
    QAction* refresh_from_master_action_;
    QAction* clear_all_action_;
    visible_updater* updater_;
};

#endif