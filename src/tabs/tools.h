#ifndef TABS_TOOLS_H
#define	TABS_TOOLS_H

class QTreeWidgetItem;
class tab_context;

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

