#ifndef BOOKMARK_TAB_H
#define	BOOKMARK_TAB_H

#include <QPointer>

#include "server_list_common_tab.h"

class QTreeWidget;
class QTreeWidgetItem;
class QAction;
class server_bookmark_list;
class visible_updater;

class bookmark_tab : public server_list_common_tab
{
    Q_OBJECT
public:
    bookmark_tab(const QString& object_name,
                 const tab_context& ctx,
                 QWidget* parent);

public slots:

protected slots:
    void do_selection_change();

private slots:
    void update_contents();
    void update_servers_info();
    void add();
    void edit_selected();
    void remove_selected();
    void refresh_all();

    void test_export();
    void test_import();
    
private:
    void update_actions();

    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    server_items items_;
    QAction* add_action_;
    QAction* edit_action_;
    QAction* remove_action_;
    QAction* refresh_selected_;
    QAction* refresh_all_;
    visible_updater* updater_;
};

#endif
