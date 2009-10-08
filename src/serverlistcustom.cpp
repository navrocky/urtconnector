#include "serverlistcustom.h"

ServerListCustom::ServerListCustom(QObject *parent)
 : QObject(parent),
   autoRefresh_(false)
{
}


ServerListCustom::~ServerListCustom()
{
}

void ServerListCustom::setOpts(ServerOptionsList * opts)
{
  opts_ = opts;
}


