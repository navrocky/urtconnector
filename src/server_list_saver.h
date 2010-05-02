#ifndef _SERVER_LIST_SAVER_H
#define	_SERVER_LIST_SAVER_H

#include <QString>

#include "pointers.h"
#include "server_list.h"

// get tuned QSettings object for server lists
qsettings_p get_server_list_settings(const QString& name);

void save_server_list(qsettings_p s, const QString& name, const server_list& list);
void save_server_list2(qsettings_p s, const QString& name, const server_list& list);
void load_server_list(qsettings_p s, const QString& name, server_list& list);
void load_server_list2(qsettings_p s, const QString& name, server_list& list);

#endif	

