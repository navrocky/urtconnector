#ifndef _SERVER_LIST_SAVER_H
#define	_SERVER_LIST_SAVER_H

#include <QString>

#include "pointers.h"
#include "server_list.h"

void save_server_list(qsettings_p s, const QString& name, const server_list& list);
void load_server_list(qsettings_p s, const QString& name, server_list& list);

#endif	

