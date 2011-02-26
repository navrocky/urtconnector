#ifndef SERVER_LIST_TAB_H
#define	SERVER_LIST_TAB_H

#include <QPointer>

#include "server_list_common_tab.h"

class QTreeWidget;
class QTreeWidgetItem;
class QAction;
class server_bookmark_list;

class server_list_tab : public server_list_common_tab
{
    Q_OBJECT
public:
    server_list_tab(const QString& object_name,
                    const tab_context& ctx,
                    QWidget* parent);

protected:
    void showEvent(QShowEvent* event);

protected slots:
    virtual void do_selection_change();

private slots:
    void update_contents();
    void add_to_favorites();
    void refresh_all();
    void clear_all();

private:
    void update_actions();

    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    bool update_contents_pended_;
    server_items items_;
    QAction* add_bookmark_action_;
    QAction* refresh_selected_action_;
    QAction* refresh_from_master_action_;
    QAction* clear_all_action_;
};

#endif