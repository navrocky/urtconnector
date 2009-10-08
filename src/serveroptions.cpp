#include "serveroptions.h"

ServerOptions::ServerOptions()
    : favorite_(false)
{
}

ServerOptions::ServerOptions(const ServerOptions & src)
{
    assign(src);
}

ServerOptions & ServerOptions::operator =(const ServerOptions & src)
{
    assign(src);
    return *this;
}

ServerOptions::~ServerOptions()
{
}

void ServerOptions::assign(const ServerOptions & src)
{
    uid_ = src.uid();
    id_ = src.id();
    name_ = src.name();
    comment_ = src.comment();
    favorite_ = src.favorite();
    rconPassword_ = src.rconPassword();
    refPassword_ = src.refPassword();
    folder_ = src.folder();
}

void ServerOptions::setId(const ServerID & val)
{
    id_ = val;
}

void ServerOptions::setName(const QString & val)
{
    name_ = val;
}

void ServerOptions::setComment(const QString & val)
{
    comment_ = val;
}

void ServerOptions::setFavorite(bool val)
{
    favorite_ = val;
}

void ServerOptions::setUid(const QUuid & val)
{
    uid_ = val;
}

void ServerOptions::setRconPassword(const QString & val)
{
    rconPassword_ = val;
}

void ServerOptions::setRefPassword(const QString & val)
{
    refPassword_ = val;
}

void ServerOptions::setFolder(const QString & val)
{
    folder_ = val;
}

ServerOptions* find_options_by_id(ServerOptionsList* list, const ServerID& id)
{
    for (ServerOptionsList::iterator it = list->begin(); it != list->end(); it++)
        if ((*it).second.id() == id)
            return &((*it).second);
    return 0;
}





