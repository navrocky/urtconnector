
#include <map>
#include <set>

// #include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/bind/apply.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <QStringList>
#include <QTimer>
#include <QUdpSocket>

#include <cl/syslog//syslog.h>
#include <common/server_id.h>
#include <common/player_info.h>
#include <common/tools.h>

#include "rcon_connection.h"

SYSLOG_MODULE(rcon);

using namespace boost;

const char answer_header_codes[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };
const char rcon_header_codes[10] = {0xff, 0xff, 0xff, 0xff, 'r', 'c', 'o', 'n', 0x20, 0x00};

const QByteArray answer_header_c( answer_header_codes );
const QByteArray rcon_header_c( rcon_header_codes );

namespace {
    class base_parser;
}

///List of unique strings
typedef std::set<QString>       Strings;

///Type to store pointer to base class of any parser
typedef boost::shared_ptr<base_parser>  Parser;
typedef std::map<std::string, Parser>   ParsersByName;

///////////////////////////////////////
//      parsers

namespace {

///base_parser = class for handling lines, that comes from remote server
struct base_parser{
    ///type of action that executes on start/stop
    typedef boost::function<void ()> Action;
    typedef boost::function<void (const QByteArray& line)> Parser;
    
    base_parser():enabled_(false){}
    virtual ~base_parser(){}

    ///Enable parser and execute apropriate function( callback )
    void enable(){ if(enabled_) return; enabled_ = true; if( start ) start(); };
    
    ///Disable parser and execute apropriate function( callback )
    void disable(){ if(!enabled_) return; enabled_ = false; if( stop ) stop(); };
    
    bool is_enabled() const { return enabled_; }

    ///Main function that handles incoming line
    virtual void operator()( const QByteArray& line ) = 0;

    ///Public callbacks executes on start/stop action
    Action start; Action stop;
private:
    bool enabled_;
};


/*!Parser that interprets incoming lines as command and stores it.
 * Quake engine does not mark begining of the command "cmdlist", so this parser must be enabled manually
*/
struct command_list_parser: base_parser{
    Strings commands;
    const QRegExp list_end_rx;
    
    command_list_parser()
        : list_end_rx("(\\d+) commands")
    {
        start = bind( &Strings::clear, boost::ref(commands) );
    }

    virtual void operator()(const QByteArray& line){
        if( !is_enabled() ) return;
                
        if( list_end_rx.exactMatch(line) )
            disable();
        else
            commands.insert( line.data() );
    }
};


/*! Parser that handles begin and end of 'status' command, and stores it */
struct player_list_parser: base_parser{
    Strings players;
    const QRegExp status_begin_rx;
    const QRegExp player_rx;
    
    player_list_parser()
        : status_begin_rx( "^[ ]*num[ ]+score[ ]+ping[ ]+name[ ]+lastmsg[ ]+address[ ]+qport[ ]+rate[ ]*$" )
        , player_rx      ( "^[ ]*\\d+[ ]+\\d+[ ]+\\d+[ ]+(.*\\^7)[ ]+.*[ ]+.*[ ]+\\d+[ ]+.*[ ]*$")
    {
        start = bind( &Strings::clear, boost::ref(players) );
    }

    virtual void operator()(const QByteArray& line){

        if( status_begin_rx.exactMatch(line) )
            enable();
        else if( line.isEmpty() )
            disable();
        else if( is_enabled() && player_rx.exactMatch(line) )
            players.insert( q3stripcolor( player_rx.cap(1) ) );
    }
};

struct map_list_parser: base_parser{
    Strings maps;
    const QString list_begin;
    const QRegExp list_end_rx;
    const QRegExp map;

    map_list_parser()
        : list_begin( "---------------" )
        , list_end_rx( "(\\d+) files listed" )
        , map("maps/(.*).bsp")
    {
        start = bind( &Strings::clear, boost::ref(maps) );
    }

    virtual void operator()(const QByteArray& line){

        if( line == list_begin )
            enable();
        else if( list_end_rx.exactMatch(line) )
            disable();
        else if( is_enabled() && map.exactMatch(line) )
            maps.insert( map.cap(1) );
    }
};

}

//////////////////////////////////
//    rcon_connection


struct rcon_connection::Pimpl{
    Pimpl( const server_id& id_, const QString& pass )
        : id(id_)
        , password(pass)
        , connected(false), waiting(false)
        , auto_update(false)
    {}
    
    void regenerate_parsers(){
        parsers.clear();
        
        parsers.push_back( bind( &base_parser::operator(), boost::ref(cmdp), _1) );
        parsers.push_back( bind( &base_parser::operator(), boost::ref(plrp), _1) );
        parsers.push_back( bind( &base_parser::operator(), boost::ref(mapp), _1) );
    }
    
    server_id id;
    QString password;
    
    bool connected;
    bool waiting;
    bool auto_update;
    QUdpSocket socket;
    
    QTimer send_timer;
    QStringList queue;
    
    command_list_parser cmdp;
    player_list_parser  plrp;
    map_list_parser     mapp;
    
    QStringList commands;
    QStringList players;
    QStringList maps;
    
    std::list<base_parser::Parser> parsers;
};

rcon_connection::rcon_connection(const server_id& id, const QString& pass, QObject* parent)
    : QObject( parent )
    , p_( new Pimpl(id, pass) )
{
    p_->send_timer.setInterval( 2000 );
    p_->send_timer.setSingleShot(false);

    connect( &p_->send_timer, SIGNAL(timeout()), SLOT(process_queue()) );
    
    connect( this, SIGNAL(received(QList<QByteArray>)), SLOT(process_input(QList<QByteArray>)) );
    
    //UdpSoket always connected, but initialization required
    connect( &p_->socket, SIGNAL( readyRead() ),   SLOT( ready_read() ) );
    
    set_server_id( p_->id );
    
    p_->cmdp.stop = bind( &rcon_connection::parser_completed, this, "commands" );
    p_->plrp.stop = bind( &rcon_connection::parser_completed, this, "players" );
    p_->mapp.stop = bind( &rcon_connection::parser_completed, this, "maps" );
        
    p_->regenerate_parsers();
}

rcon_connection::~rcon_connection()
{}

void rcon_connection::set_server_id(const server_id& id_)
{
    p_->id = id_;
    p_->socket.disconnectFromHost();
    p_->socket.connectToHost( p_->id.ip_or_host(), p_->id.port() );
}


void rcon_connection::set_password(const QString& pass)
{ p_->password = pass; }

void rcon_connection::set_auto_update(bool b)
{
    if( p_->auto_update == b ) return;
    
    p_->auto_update = b;
    
    if( p_->auto_update ){
        commands(); players(), maps();
    }
}

const QStringList& rcon_connection::commands()
{
    p_->cmdp.enable();
    
    send_command( "cmdlist" );
        
    if(p_->auto_update) QTimer::singleShot(60000, this, SLOT( commands() ) );
        
    return p_->commands;
}

const QStringList& rcon_connection::players()
{
    send_command( "status" );
    if(p_->auto_update) QTimer::singleShot(10000, this, SLOT( players() ) );
    
    return p_->players;
}

const QStringList& rcon_connection::maps()
{
    send_command( "fdir *.bsp" );
    if(p_->auto_update) QTimer::singleShot(60000, this, SLOT( maps() ) );

    return p_->maps;
}

void rcon_connection::kick_player(const player_info& player)
{
    send_command( QString("kick \"%1\"").arg( q3stripcolor(player.nick_name() ) ) );
}

void rcon_connection::set_map(const QString& map)
{
    send_command( QString("map \"%1\"").arg( map ) );
}

void rcon_connection::send_command( const QString& command )
{
    if( command.isEmpty() )
        return;
    
    p_->queue << command;
    
    if( !p_->send_timer.isActive() )
        QTimer::singleShot(0, this, SLOT( process_queue() ) );
}

void rcon_connection::ready_read()
{
    QByteArray data = p_->socket.readAll();
    LOG_DEBUG << "Recieved from %1: %2", p_->id.address().toStdString(),  data.constData();

    p_->waiting = false;
    
    emit received( data.split('\n') );   
}

void rcon_connection::process_input(const QList< QByteArray >& data)
{
    BOOST_FOREACH( const QByteArray& line, data ){
        if( line.startsWith( answer_header_c ) )
        {
            QByteArray command = line.mid( answer_header_c.size(), -1 );
            if( command == "print" )
                continue;
            else
                throw std::runtime_error("unknown response command in rcon");
        }

        if ( line == "Bad rconpassword." )
            emit bad_password( p_->id );

        std::for_each( p_->parsers.begin(), p_->parsers.end(), bind( apply<void>(), _1, line) );
    }
}

void rcon_connection::process_queue()
{
    set_state( !p_->waiting );
    
    if( p_->queue.isEmpty() ) return;
   
    if ( p_->socket.state() == QAbstractSocket::ConnectedState )
    {
        QString current = p_->queue.front();
        p_->queue.pop_front();
        
        QString cmd = QString( "%1 %2 %3" )
            .arg( rcon_header_c.data() )
            .arg( p_->password )
            .arg( qPrintable(current) );

        LOG_DEBUG << "Sended to %1: %2", p_->id.address().toStdString(), cmd.toStdString();
        p_->socket.write( cmd.toStdString().data() );
        p_->waiting = true; //waiting for answer
    }

    p_->send_timer.start();
}

void rcon_connection::set_state(bool connected)
{
    if ( p_->connected && !connected )
    {
        LOG_HARD << "Connection failed to %1", p_->id.address().toStdString();
        emit connection_changed( true );
    }
    else if( !p_->connected && connected )
    {
        LOG_HARD << "Connected to %1", p_->id.address().toStdString();
        emit connection_changed( false );
    }
    
    p_->connected = connected;
}

void rcon_connection::parser_completed(const QString& name)
{
    if( name == "commands" )
    {
        QStringList new_list;
        std::copy( p_->cmdp.commands.begin(), p_->cmdp.commands.end(), std::back_inserter(new_list) );
        if( new_list != p_->commands )
        {
            p_->commands = new_list;
            emit commands_changed(p_->commands);
        }
    }
    else if( name == "players" )
    {
        QStringList new_list;
        std::copy( p_->plrp.players.begin(), p_->plrp.players.end(), std::back_inserter(new_list) );
        if( new_list != p_->players )
        {
            p_->players = new_list;
            emit players_changed(p_->players);
        }
    }
    else if( name == "maps" )
    {
        QStringList new_list;
        std::copy( p_->mapp.maps.begin(), p_->mapp.maps.end(), std::back_inserter(new_list) );
        if( new_list != p_->maps )
        {
            p_->maps = new_list;
            emit maps_changed(p_->maps);
        }
    }
}









