#include "server_options.h"

////////////////////////////////////////////////////////////////////////////////
// server_bookmark

bool server_bookmark::is_empty() const
{
    return id.is_empty();
}

////////////////////////////////////////////////////////////////////////////////
// server_bookmark_list

server_bookmark_list::server_bookmark_list(QObject* parent)
: QObject(parent)
{
}

void server_bookmark_list::add(const server_bookmark& bm)
{
    list_[bm.id] = bm;
    emit changed();
}

void server_bookmark_list::change(const server_id& old, const server_bookmark& bm)
{
    if (bm.id != old)
        list_.remove(old);
    list_[bm.id] = bm;
    emit changed();
}

void server_bookmark_list::remove(const server_id& id)
{
    list_.remove(id);
    emit changed();
}

const server_bookmark& server_bookmark_list::get(const server_id& id) const
{
    bookmark_map_t::const_iterator it = list_.find(id);
    if (it == list_.end())
    {
        static const server_bookmark empty;
        return empty;
    }
    else
        return it.value();
}

void server_bookmark_list::remove_all()
{
    list_.clear();
    emit changed();
}



