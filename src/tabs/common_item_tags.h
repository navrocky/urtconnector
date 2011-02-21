#ifndef COMMON_ITEM_TAGS_H
#define	COMMON_ITEM_TAGS_H

#include <Qt>

// const int c_info_role = Qt::UserRole;
static const int c_id_role = Qt::UserRole + 1;

// (int) used to detect item changes
static const int c_stamp_role = Qt::UserRole + 2;

//FIXME move to shared place USED in history_tab
static const int c_suppress_role = Qt::UserRole + 11;


#endif	/* COMMON_ITEM_TAGS_H */

