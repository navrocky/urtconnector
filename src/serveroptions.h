#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>

#include "server_id.h"

class server_options
{
public:
    server_id id;
    QString name;
    QString comment;
    QString password;
    QString rcon_password;
    QString ref_password;
    QString folder;
};

// List of ServerOptions
typedef std::map<server_id, server_options> server_fav_list;

#endif
