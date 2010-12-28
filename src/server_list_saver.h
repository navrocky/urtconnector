#ifndef _SERVER_LIST_SAVER_H
#define	_SERVER_LIST_SAVER_H

#include <QString>

#include "pointers.h"

void init_database();
void save_server_list(const QString& name, server_list_p list);
void load_server_list(const QString& name, server_list_p list);

#endif	

