#ifndef COMMON_ITEM_TAGS_H
#define	COMMON_ITEM_TAGS_H

#include <Qt>

// const int c_info_role = Qt::UserRole;
static const int c_id_role = Qt::UserRole + 1;

// (int) used to detect item changes
static const int c_stamp_role = Qt::UserRole + 2;

// pointer to item delegate (used by proxy_delegate)
static const int c_delegate_ptr_role = Qt::UserRole + 3;

//FIXME move to shared place USED in history_tab
static const int c_suppress_role = Qt::UserRole + 11;

static const int c_history_role = Qt::UserRole + 12;

static const int c_friend_role = Qt::UserRole + 13;

#endif	/* COMMON_ITEM_TAGS_H */

