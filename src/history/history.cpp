
#include <boost/bind.hpp>

#include <QSettings>
#include <QDateTime>

#include "app_options.h"
#include "settings/settings.h"

#include "history.h"

history::history( QObject* parent )
    : QObject( parent )
    , max_days_( 0 )
{
    options_refresh();
    load();
    
    connect( this, SIGNAL( changed() ), this, SLOT( save() ) );
}

void history::add( const server_id& id, const QString& server_name,
                   const QString& player_name, const QString& password )
{
    add(history_item(id, server_name, password, player_name, QDateTime::currentDateTime()));
}

void history::add( const history_item& item )
{
    list_.push_back(item);
    emit changed();
}

void history::remove( const history_item& item )
{
    history_list_t::iterator it = std::find( list_.begin(), list_.end(), item );
    
    if( it == list_.end() )
        return;
    
    list_.erase( it );
    emit changed();
}

void history::clear()
{
    list_.clear();
    emit changed();
}

void history::options_refresh()
{
    app_settings s;
    list_.set_capacity( s.number_in_history() );
    max_days_ = s.days_in_history();
    emit changed();
}

void history::save() const
{
    history_list_t::const_iterator it = list_.begin();

    base_settings::qsettings_p history_file = base_settings().get_settings( "history" );
    
    history_file->beginWriteArray("history");

    for( int index = 0; it != list_.end(); ++it, ++index ){
        history_file->setArrayIndex( index );
        history_file->setValue("timestamp",   it->timestamp());
        history_file->setValue("server_name", it->server_name());
        history_file->setValue("address",     it->id().address());
        history_file->setValue("password",    it->password());
        history_file->setValue("player_name", it->player_name());
    }
    
    history_file->endArray();
}

void history::load()
{
    list_.clear();
    QDateTime timestamp;
    QString server_name, address, password, player_name;
    
    base_settings::qsettings_p history_file = base_settings().get_settings( "history" );
    
    int size = history_file->beginReadArray("history");
    
    for (int i = 0; i < size; i++) {
        history_file->setArrayIndex(i);
        //FIXME date_time used for backward compatibility. remove it when 0.8.0 released
        timestamp   = history_file->value("timestamp", history_file->value("date_time").toDateTime() ).toDateTime();
        server_name = history_file->value("server_name").toString();
        address     = history_file->value("address").toString();
        password    = history_file->value("password").toString();
        player_name = history_file->value("player_name").toString();
        
        list_.push_back( history_item(server_id(address), server_name, password, player_name, timestamp) );
    }
    
    history_file->endArray();
    
    if( max_days_ )
        drop_old();
    else
        emit changed();
}

void history::drop_old()
{
    list_.erase(
        std::remove_if( list_.begin(), list_.end(), bind( &history_item::timestamp, _1) < QDateTime::currentDateTime().addDays( -max_days_ ) ),
        list_.end()
    );
    
    emit changed();
}

