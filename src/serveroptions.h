#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>

#include "serverid.h"

class ServerOptions
{
public:
    server_id id;
    QString name;
    QString comment;
    QString password;
    QString rconPassword;
    QString refPassword;
    QString folder;
};

// List of ServerOptions
typedef std::map<server_id, ServerOptions> ServerOptionsList;

#endif
