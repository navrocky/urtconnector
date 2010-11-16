#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>

#include "common/server_id.h"

/*! Server options, bookmark, favorite */
class server_options
{
public:
    server_id id;
    QString name;
    QString comment;
    QString password;
    QString rcon_password;
    QString ref_password;
};

/*! List of server_options */
typedef std::map<server_id, server_options> server_fav_list;

#endif
