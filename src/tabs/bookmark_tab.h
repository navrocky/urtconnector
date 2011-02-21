#ifndef BOOKMARK_TAB_H
#define	BOOKMARK_TAB_H

#include <QPointer>

#include "server_list_common_tab.h"

class QTreeWidget;
class QTreeWidgetItem;
class QAction;
class server_bookmark_list;

class bookmark_tab : public server_list_common_tab
{
    Q_OBJECT
public:
    bookmark_tab(const QString& object_name,
                 const tab_context& ctx,
                 QWidget* parent);

public slots:

protected slots:

protected:

private slots:
    void update_contents();
    void add();
    void edit_selected();
    void remove_selected();
    void refresh_all();

private:
    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    int visible_count_;
    server_items items_;
    QAction* add_action_;
    QAction* edit_action_;
    QAction* remove_action_;
    QAction* refresh_selected_;
    QAction* refresh_all_;
};

#endif
