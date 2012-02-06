
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include "server_bookmark.h"

////////////////////////////////////////////////////////////////////////////////
// server_bookmark

server_bookmark::server_bookmark(const server_id& id,
                                 const QString& name,
                                 const QString& comment,
                                 const QString& password,
                                 const QString& rcon_password,
                                 const QString& ref_password)
{
    d->id = id;
    d->name = name;
    d->comment = comment;
    d->password = password;
    d->rcon_password = rcon_password;
    d->ref_password = ref_password;
}

const server_bookmark& server_bookmark::empty()
{
    static const server_bookmark emp;
    return emp;
}


////////////////////////////////////////////////////////////////////////////////
// server_bookmark_list

server_bookmark_list::server_bookmark_list()
    : QObject(0)
{}

void server_bookmark_list::add( const server_bookmark& bm )
{
    server_bookmark old_bm = get( bm.id() );
    emit changed( old_bm, list_.insert( std::make_pair(bm.id(), bm) ).first->second );
}

void server_bookmark_list::change( const server_id& old, const server_bookmark& bm )
{
    if ( bm.id() != old ) remove( old );
    
    add( bm );
}

void server_bookmark_list::change( const server_bookmark& bm )
{
    add( bm );
}

void server_bookmark_list::remove( const server_id& id )
{
    server_bookmark old_bm = get( id );
    list_.erase( id );
    emit changed( old_bm, server_bookmark::empty() );
}

void server_bookmark_list::remove( const server_bookmark& bm )
{ list_.erase( bm.id() ); }


const server_bookmark& server_bookmark_list::get( const server_id& id ) const
{
    bookmark_map_t::const_iterator it = list_.find( id );
    
    if( it != list_.end() )
        return it->second;
    else
        return server_bookmark::empty();
}

void server_bookmark_list::clear()
{
    BOOST_FOREACH(const bookmark_map_t::value_type val, list_) {
        emit changed( val.second, server_bookmark::empty() );
    }
    list_.clear();
}

