#ifndef SERVER_LIST_COMMON_TAB_H
#define	SERVER_LIST_COMMON_TAB_H

#include <QPointer>

#include <common/server_bookmark.h>

#include "filtered_tab.h"

class QTreeWidget;
class QTreeWidgetItem;

// Common tab widget for bookmarks and all server list
class server_list_common_tab : public filtered_tab
{
    Q_OBJECT
public:
    server_list_common_tab(const QString& object_name,
                           const QString& caption,
                           const tab_context& ctx,
                           bool complex_filter,
                           QWidget* parent);

    virtual server_id selected_server() const;
    virtual void set_selected_server(const server_id&);
    server_id_list selection() const;

public slots:
    virtual void save_state();
    virtual void load_state();

protected slots:
    virtual void filter_changed();
    void refresh_selected();
    virtual void do_selection_change();

protected:
    void set_total_count(int);
    QTreeWidget* tree() const {return tree_;}

private slots:
    void update_caption();

private:
    void set_visible_count(int);
    
    QTreeWidget* tree_;
    int visible_count_;
    int total_count_;
    QString caption_;
};


#endif	/* SERVER_LIST_COMMON_TAB_H */

