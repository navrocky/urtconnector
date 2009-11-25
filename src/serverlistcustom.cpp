#include "serverlistcustom.h"

ServerListCustom::ServerListCustom(QObject *parent)
 : QObject(parent),
   state_(0),
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


