#ifndef _SERVER_LIST_SAVER_H
#define	_SERVER_LIST_SAVER_H

#include <QString>

#include "pointers.h"

void save_server_list(qsettings_p s, const QString& name, server_list_p list);
void load_server_list(qsettings_p s, const QString& name, server_list_p list);

// QByteArray save_server_list2(const server_list& list);
// void load_server_list2(server_list& list, const QByteArray& ba);

#endif	

