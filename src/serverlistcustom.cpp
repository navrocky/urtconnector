#include "serverlistcustom.h"

serv_list_custom::serv_list_custom(QObject *parent)
 : QObject(parent),
   state_(0),
   autoRefresh_(false)
{
}


serv_list_custom::~serv_list_custom()
{
}

void serv_list_custom::setOpts(ServerOptionsList * opts)
{
  opts_ = opts;
}


