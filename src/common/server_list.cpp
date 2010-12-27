#include <algorithm>

#include "server_list.h"

server_list::server_list(QObject* parent)
: QObject(parent)
, state_(0)
{
}

void server_list::state_changed()
{
    state_++;
    emit changed();
}

server_info_p server_list::get(const server_id& id)
{
    server_info_list::iterator it = list_.find(id);
    if (it == list_.end())
    {
        return server_info_p();
    }
    else
        return it->second;
}

void server_list::add(server_info_p si)
{
    list_[si->id] = si;
    state_changed();
}

void server_list::remove_all()
{
    list_.clear();
    state_changed();
}

void server_list::remove_selected(const server_id_list& ids)
{
    foreach (const server_id& id, ids)
    {
        server_info_list::iterator it = list_.find(id);
        if (it != list_.end())
            list_.erase(it);
    }
    state_changed();
}

void server_list::remove_old()
{
    // TODO remove if update stamp is too old
    state_changed();
}


