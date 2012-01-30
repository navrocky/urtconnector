#ifndef TABS_TOOLS_H
#define	TABS_TOOLS_H

#include <QTreeWidgetItem>

class tab_context;

// override sorting by ping
class server_info_item : public QTreeWidgetItem
{
public:
    server_info_item(QTreeWidget* view);
    server_info_item(QTreeWidgetItem* parent);

    virtual bool operator<(const QTreeWidgetItem &other) const;
};

// adapter for smart_update_tree_contents() algorithm
struct server_info_item_adapter
{
    QTreeWidgetItem * create_item(QTreeWidget* tree, QTreeWidgetItem * parent_item) const
    {
        return ( parent_item)
                ? new server_info_item(parent_item)
                : new server_info_item(tree);
    }

    void remove_item(QTreeWidgetItem * item) const
    {
        delete item;
    }
};

// server_id in data(c_id_role)

// Columns:
//  0 - status
//  1 - name
//  2 - country
//  3 - ping
//  4 - game mode
//  5 - map
//  6 - players
//  7 - address

void update_server_info_item(const tab_context& ctx, QTreeWidgetItem* item);

#endif	/* TOOLS_H */

