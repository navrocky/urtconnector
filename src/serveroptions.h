#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>

#include "serverid.h"

class ServerOptions
{
public:
    ServerOptions();

    ServerID id;
    QString name;
    QString comment;
    QString rconPassword;
    QString refPassword;
    QString folder;
};

// List of ServerOptions
typedef std::map<ServerID, ServerOptions> ServerOptionsList;

#endif
