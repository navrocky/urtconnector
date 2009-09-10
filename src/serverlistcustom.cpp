#include "serverlistcustom.h"

ServerListCustom::ServerListCustom(QObject *parent)
 : QObject(parent),
   state_(0),
   listMutex_(QMutex::Recursive),
   autoRefresh_(false)
{
}


ServerListCustom::~ServerListCustom()
{
}




ServerListAccess::ServerListAccess(ServerListCustom* sl) :
    lock_(&(sl->listMutex_)),
    list_(sl->list_)
{
}