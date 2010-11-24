
#include <set>
#include <list>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <QLabel>
#include <QTimer>
#include <QUdpSocket>
#include <QTime>
#include <QTreeView>
#include <QStandardItemModel>

#include <cl/syslog/syslog.h>

#include "common/server_id.h"
#include "server_options.h"
#include "tools.h"

#include "ui_rcon.h"
#include "rcon.h"
#include "rcon/rcon_completer.h"
#include <boost/concept_check.hpp>


SYSLOG_MODULE("rcon");

using namespace boost;
using namespace boost::assign;

struct base_parser;

const char answer_header_codes[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };
const char rcon_header_codes[10] = {0xff, 0xff, 0xff, 0xff, 'r', 'c', 'o', 'n', 0x20, 0x00};

const QByteArray answer_header_c( answer_header_codes );
const QByteArray rcon_header_c( rcon_header_codes );

///Comand to remote server and flag of answer visibility
typedef std::pair<std::string, bool>    CommandOpt;
typedef std::list<CommandOpt>           CommandQueue;

///Type to store pointer to base class of any parser
typedef boost::shared_ptr<base_parser>  Parser;
typedef std::map<std::string, Parser>   ParsersByName;

///List of unique strings
typedef std::set<std::string>           Strings;
typedef std::map<std::string, Strings>  ExpandersByName;

///base_parser = class for handling lines, that coms from remote server
struct base_parser{
    ///type of action that executes on start/stop
    typedef boost::function<void ()> Action;
    
    base_parser():enabled_(false){}
    virtual ~base_parser(){}

    ///Enable parser and execute apropriate function( callback )
    void enable(){ enabled_ = true; if( start ) start(); };
    
    ///Disable parser and execute apropriate function( callback )
    void disable(){ enabled_ = false; if( stop ) stop(); };
    
    bool is_enabled() const { return enabled_; }

    ///Main function that handles incoming line
    virtual void operator()( const QByteArray& line ) = 0;

    ///Public callbacks executes on start/stop action
    Action start; Action stop;
private:
    bool enabled_;
};

/*!Parser that interprets incoming lines as command and stores it.
 * Quake engine does not mark begining of the command "cmdlist", so this parser mut be enabled manually
*/
struct command_list_parser: base_parser{
    Strings& commands;
    const QRegExp list_end_rx;
    
    command_list_parser( Strings& c )
        : commands(c)
        , list_end_rx("(\\d+) commands")
    {}

    virtual void operator()(const QByteArray& line){
        if( !is_enabled() ) return;

        if( list_end_rx.exactMatch(line) )
            disable();
        else
            commands.insert( line.data() );
    }
};

/*! Parser that handles begin and end of players list, and stores it */
struct player_list_parser: base_parser{
    Strings& players;
    const QString list_begin;
    const QString list_end;
    const QRegExp player_rx;
    
    player_list_parser( Strings& p )
        : players(p)
        , list_begin( "Current players:" )
        , list_end  ( "End current player list." )
        , player_rx ("\\s+(\\d+):\\s+\\[(.*)\\]")
    {}

    virtual void operator()(const QByteArray& line){

        if( line == list_begin )
            enable();
        else if( line == list_end )
            disable();
        else if( is_enabled() && player_rx.exactMatch(line) )
            players.insert( player_rx.cap(2).toStdString() );
    }
};

/*! Handling of map list*/
struct map_list_parser: base_parser{
    Strings& maps;
    const QString list_begin;
    const QRegExp list_end_rx;
    const QRegExp map;

    map_list_parser( Strings& m )
        : maps(m)
        , list_begin( "---------------" )
        , list_end_rx( "(\\d+) files listed" )
        , map("maps/(.*).bsp")
    {}

    virtual void operator()(const QByteArray& line){

        if( line == list_begin )
            enable();
        else if( list_end_rx.exactMatch(line) )
            disable();
        else if( is_enabled() && map.exactMatch(line) )
            maps.insert( map.cap(1).toStdString() );
    }
};


///Type thar represents Item in completition model
struct Item{
    ///item which has static childs and dynamic expanders
    QStandardItem*  item;
    ///list of static childs
    Strings         st_list;
    ///list of expander names
    Strings         ex_list;
};

typedef std::list<Item> Items;


struct rcon::Pimpl{
    Pimpl( const server_id& id, const server_options& options )
        : id(id), options(options)
        , connected(false), waiting(false)
    {}
    void init() {
      
        QFont f("");
        f.setStyleHint(QFont::TypeWriter);
        ui.output->setFont( f );
        ui.input->setFont( f );

        status = new QLabel(0);
        status->setFixedSize( QSize(16, 16) );

        //moving pixmap to Right-Up corner
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget(status);
        vl->addStretch();

        QHBoxLayout* hl = new QHBoxLayout(ui.output);
        hl->addStretch();
        hl->addLayout( vl );

        ui.output->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        
        queue_timer.setInterval( 2000 );
        queue_timer.setSingleShot( false );

        send_timer.setInterval( 2000 );
        send_timer.setSingleShot( true );
    }

    void update_colors(){
        rcon_settings s;
        if( s.adaptive_pallete() )
        {
            QPalette p = ui.output->palette();
            boost::assign::insert(colors)
                (rcon_settings::Background, p.color( QPalette::Base ) )
                (rcon_settings::Text,       p.color( QPalette::Text ) )
                (rcon_settings::Command,    choose_for_background( Qt::yellow, p.color( QPalette::Base ) ) )
                (rcon_settings::Info,       choose_for_background( Qt::cyan,   p.color( QPalette::Base ) ) )
                (rcon_settings::Error,      choose_for_background( Qt::red,    p.color( QPalette::Base ) ) );
        }
        else
        {
            boost::assign::insert(colors)
                (rcon_settings::Background, s.color(rcon_settings::Background) )
                (rcon_settings::Text,       s.color(rcon_settings::Text)       )
                (rcon_settings::Command,    s.color(rcon_settings::Command)    )
                (rcon_settings::Info,       s.color(rcon_settings::Info)       )
                (rcon_settings::Error,      s.color(rcon_settings::Error)      );
        }
    }
    
    Ui_rcon ui;
    server_id id;
    server_options options;
    QUdpSocket socket;
    bool connected;
    bool waiting;
   
    QLabel* status;

    std::map<rcon_settings::Color, QColor> colors;

    //last sended command
    CommandOpt      current;
    CommandQueue    queue;
    
    QTimer          queue_timer;
    QTimer          send_timer;

    //TODO port to cl::timer
    QTime           last_send;

    QStandardItemModel  model;

    Items               items;
    ParsersByName       parsers;
    ExpandersByName     expanders;
};



Items::iterator get_item( Items& items, QStandardItem* item ){
    Items::iterator it = std::find_if( items.begin(), items.end(), bind(&Item::item, _1) == item );
    if( it == items.end() )
    {
        it = items.insert( items.end(), Item() );
        it->item = item;
    }
    return it;
}

struct find_by_expander: std::binary_function<const Item&, const std::string&, bool>{
    bool operator()( const Item& item, const std::string& expander ){
        return std::find( item.ex_list.begin(), item.ex_list.end(), expander ) != item.ex_list.end();
    }
};

rcon::rcon(QWidget* parent, const server_id& id, const server_options& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
    p_->init();

    //TODO move to config file
    QStringList lst;
    lst << "%commands%" <<  "kick %players%" << "map %maps%" << "forceteam blue,red %players%" ;

    //FIXME make separate
    BOOST_FOREACH ( const QString& cmd_str, lst ) {

        QStringList splitted = cmd_str.split(" ");

        QStandardItem* p_item = p_->model.invisibleRootItem();
        for (uint i=0; i< splitted.size(); ++i) {
            Items::iterator it = get_item( p_->items, p_item );

            QRegExp rx("%(.*)%");
            QRegExp rx2("(.*),(.*)");
            if( rx.exactMatch( splitted[i]) )
                it->ex_list.insert( rx.cap(1).toStdString() );
            else if( rx2.exactMatch( splitted[i]) )
            {
                BOOST_FOREACH( const QString& str, splitted[i].split(",") )
                {
                    it->st_list.insert( str.toStdString() );
                    QStandardItem* item = new QStandardItem( str );
                    p_item->appendRow( item );
                    it = get_item( p_->items, item );
                    //FIXME error...
                    p_item = item;
                }
            }
            else
            {
                it->st_list.insert( splitted[i].toStdString() );
                QStandardItem* item = new QStandardItem( splitted[i] );
                p_item->appendRow( item );
                it = get_item( p_->items, item );
                p_item = item;
            }
        }

    }

//     QTreeView* v = new QTreeView(0);
//     v->show();
//     v->setModel( &p_->model );

    //Configuring parsers
    insert( p_->parsers )
        ( "commands" , Parser( new command_list_parser( p_->expanders["commands"] ) ) )
        ( "players"  , Parser( new player_list_parser ( p_->expanders["players"]  ) ) )
        ( "maps"     , Parser( new map_list_parser    ( p_->expanders["maps"]     ) ) )
    ;

    p_->parsers["commands"]->start = bind( &Strings::clear, ref( p_->expanders["commands"] ) );
    p_->parsers["commands"]->stop  = bind( &rcon::refresh_expander, this,  "commands" );

    p_->parsers["players"]->start  = bind( &Strings::clear, ref( p_->expanders["players"] ) );
    p_->parsers["players"]->stop   = bind( &rcon::refresh_expander, this,  "players" );

    p_->parsers["maps"]->start     = bind( &Strings::clear, ref( p_->expanders["maps"] ) );
    p_->parsers["maps"]->stop      = bind( &rcon::refresh_expander, this,  "maps" );

    
    rcon_completer* rc = new rcon_completer(p_->ui.input, &p_->model, this );
    rc->setSeparator(" ");

    
    //UdpSoket always connected, but initialization required
    connect( &p_->socket, SIGNAL( connected() ),   SLOT( connected() ) );
    connect( &p_->socket, SIGNAL( readyRead() ),   SLOT( ready_read() ) );

    connect( &p_->queue_timer, SIGNAL( timeout () ), this, SLOT( process_queue() ) );
    connect( &p_->send_timer,  SIGNAL( timeout()  ), this, SLOT( send_timeout()  ) );

    connect( p_->ui.input, SIGNAL( returnPressed() ), SLOT( input_enter_pressed() ));

    update_settings();
    
    p_->socket.connectToHost( p_->id.ip_or_host(), p_->id.port() );
}

rcon::~rcon()
{}

void rcon::send_command( const QString& command, bool supress )
{
    if( command.isEmpty() )
        return;
    
    p_->queue.push_back( CommandOpt(command.toStdString(), supress) );
    if ( abs(p_->last_send.msecsTo( QTime::currentTime() )) >= 2000 )
    {
        process_queue();
        p_->queue_timer.start();
    }
}


void rcon::ready_read()
{
    QByteArray data = p_->socket.readAll();
    LOG_DEBUG << format( "%1% - recieved: %2%" ) % p_->id.address().toStdString() % data.constData();

    BOOST_FOREACH( const QByteArray& line, data.split('\n') ) {
        parse_line( line );
    }

    p_->send_timer.stop();
    p_->waiting = false;
    send_timeout();
}

void rcon::update_settings()
{
    p_->update_colors();
    QPalette p = p_->ui.output->palette();
    p.setColor( QPalette::Base, p_->colors[rcon_settings::Background] );
    p_->ui.output->setPalette( p );
    p_->ui.output->setAutoFillBackground( !rcon_settings().adaptive_pallete() );

//    p.setColor( QPalette::Text, p_->colors[rcon_settings::Command] );
//    p_->ui.input->setPalette( p );
//    p_->ui.input->setAutoFillBackground( !rcon_settings().adaptive_pallete() );
}


void rcon::connected()
{
    p_->queue_timer.start();

    p_->parsers["commands"]->enable();
    send_command( "cmdlist", true );
    
    refresh_players();
    refresh_maps();
}


void rcon::input_enter_pressed()
{
    print( Command, p_->ui.input->text() );
    send_command( p_->ui.input->text() );
    p_->ui.input->clear();
}

void rcon::send_timeout()
{
    set_state( !p_->waiting );
    p_->waiting = false;
}

void rcon::set_state( bool connected)
{
    if ( !connected )
    {
        LOG_HARD << p_->id.address().toStdString() << " - connection failed";
        p_->status->setPixmap( QPixmap(":/icons/icons/status-offline.png") );
        print( Info, tr("connection failed") );
    }
    else if( !p_->connected && connected )
    {
        LOG_HARD << p_->id.address().toStdString() << " - connected";
        p_->status->setPixmap( QPixmap(":/icons/icons/status-online.png") );
        print( Info, tr("connected") );
    }
    
    p_->connected = connected;
}

void rcon::parse_line( const QByteArray& line )
{
    if( line.startsWith( answer_header_c ) )
    {
        QByteArray command = line.mid( answer_header_c.size(), -1 );
        if( command == "print" )
            return;
        else
            throw std::runtime_error("uncnown response command in RCon");
    }

    if( p_->current.second == false ){
        print( Simple, line );
    }

    BOOST_FOREACH( ParsersByName::value_type& parser, p_->parsers )
        parser.second->operator()( line );
}


void rcon::print( TextType type, const QString & text )
{
    if( !text.isEmpty() )
        p_->ui.output->append( colorize_string(type, text) );
}

QString rcon::colorize_string( rcon::TextType type, const QString& text ) const
{
    switch (type)
    {
        case rcon::Command: return colorize( text, p_->colors[rcon_settings::Command] );
        case rcon::Simple:  return colorize( text, p_->colors[rcon_settings::Text] );
        case rcon::Info:    return colorize( text, p_->colors[rcon_settings::Info] );
        case rcon::Error:   return colorize( text, p_->colors[rcon_settings::Error] );
        default:
            return text;
    }    
}

void rcon::refresh_players()
{
    send_command( "playerlist", true );
    //refresh playerlist every 10 seconds
    QTimer::singleShot( 10000, this, SLOT( refresh_players() ) );
}

void rcon::refresh_maps()
{
    send_command( "fdir *.bsp", true );
    //refresh maplist every 60 seconds
    QTimer::singleShot( 50000, this, SLOT( refresh_maps() ) );
}

void rcon::refresh_expander( const std::string& exp )
{
    std::for_each(
        make_filter_iterator( bind( find_by_expander(), _1, exp ), p_->items.begin(), p_->items.end() ),
        make_filter_iterator( bind( find_by_expander(), _1, exp ), p_->items.end()  , p_->items.end() ),
        bind( &rcon::update_item, this, _1)
    );
}


void rcon::process_queue()
{
    if( p_->queue.empty() ) return;

    p_->current = p_->queue.front();
    p_->queue.pop_front();
   
    QString cmd = QString( "%1 %2 %3" )
        .arg( rcon_header_c.data() )
        .arg( p_->options.rcon_password )
        .arg( p_->current.first.data() );

    p_->waiting = true;
    p_->send_timer.start();
    
    LOG_DEBUG << format( "%1% - send: %2%" ) % p_->id.address().toStdString() % cmd.toStdString();
    p_->socket.write( cmd.toStdString().data() );

    p_->last_send = QTime::currentTime();
}

void rcon::update_item(const Item& item)
{
    QList<QStandardItem*> to_delete;

    //List of all autocompletition elements of item
    Strings full_list;
    //...static items
    std::copy( item.st_list.begin(), item.st_list.end(), std::inserter( full_list, full_list.end() ) );

    //...dynamically expanded items
    BOOST_FOREACH( const std::string& exp, item.ex_list )
        std::copy( p_->expanders[exp].begin(), p_->expanders[exp].end(), std::inserter( full_list, full_list.end() ) );
  

    //checking QStandardItemModel structure
    QStandardItem* child(0);
    Strings::iterator cmd;
    for( int i = 0; i < item.item->rowCount(); ++i ) {
        child = item.item->child( i );

        if( cmd = full_list.find( child->text().toStdString() ), cmd != full_list.end() ) //Item exixst all ok
            full_list.erase( cmd );
        else // Item does not exist and model item must be removed
            to_delete.push_back( child );
    }

    BOOST_FOREACH( const QStandardItem* it, to_delete   )
        p_->model.removeRow( it->row(), item.item->index() );

    BOOST_FOREACH( const std::string& cmd, full_list )
        item.item->appendRow( new QStandardItem( cmd.data() ) );
}




