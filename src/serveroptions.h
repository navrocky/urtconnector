#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>
#include <QUuid>

#include "serverid.h"

class ServerOptions
{
public:
    ServerOptions();

    QUuid uid;

    ServerID id;

    QString name;

    QString comment;

    bool favorite;

    QString rconPassword;

    QString refPassword;

    QString folder;
};


/// List of ServerOptions
typedef std::map<QUuid, ServerOptions> ServerOptionsList;

#endif
