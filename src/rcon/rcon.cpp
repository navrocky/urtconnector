
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

#include <common/server_id.h>
#include "../server_bookmark.h"
#include "tools.h"

#include "ui_rcon.h"
#include "rcon.h"
#include "rcon_completer.h"

SYSLOG_MODULE(rcon)

using namespace boost;
using namespace boost::assign;

struct base_parser;

const char answer_header_codes[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };
const char rcon_header_codes[10] = {0xff, 0xff, 0xff, 0xff, 'r', 'c', 'o', 'n', 0x20, 0x00};

const QByteArray answer_header_c( answer_header_codes );
const QByteArray rcon_header_c( rcon_header_codes );

const QString exp_rx_c( "%(\\w+)%" );
const QString group_rx_c( "\\{(.*)\\}" );
const QString word_rx_c( "\\w+" );

const QString config_rx_c( QString("%1|%2|%3").arg(exp_rx_c).arg(group_rx_c).arg(word_rx_c) );

///Comand to remote server and flag of answer visibility
typedef std::pair<std::string, bool>    CommandOpt;
typedef std::list<CommandOpt>           CommandQueue;

///Type to store pointer to base class of any parser
typedef boost::shared_ptr<base_parser>  Parser;
typedef std::map<std::string, Parser>   ParsersByName;

///List of unique strings
typedef std::set<QString>           Strings;
typedef std::map<QString, Strings>  ExpandersByName;

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
            players.insert( player_rx.cap(2) );
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
            maps.insert( map.cap(1) );
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
    ///If this item is dynamically expanded holds completition config
    std::list<QString>         config_list;
};

typedef std::list<Item> Items;


struct rcon::Pimpl{
    Pimpl( const server_id& id, const server_bookmark& options )
        : id(id), options(options)
        , connected(false), waiting(false)
    {}
    void init() {

#if defined(Q_OS_UNIX)
        QFont f("terminus");
#elif defined(Q_OS_WIN)
        QFont f("");
#endif
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
        if( !s.custom_colors() )
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
    server_bookmark options;
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
    bool operator()( const Item& item, const QString& expander ){
        return std::find( item.ex_list.begin(), item.ex_list.end(), expander ) != item.ex_list.end();
    }
};

QStringList split( const QString& str ){
    static const QRegExp rx( config_rx_c );
    QStringList ret;
    int pos = 0;
    while ( ( pos = rx.indexIn(str, pos) ) != -1 ) {
        pos += rx.cap(0).size();
        ret << rx.cap(0);
    }
    return ret;
}

///Function for recursevly creating items from config
template <typename Iterator>
void create_items( QStandardItem* parent, Iterator begin, Iterator end, Items& items, bool static_item = true){
    static const QRegExp expander_rx(exp_rx_c);
    static const QRegExp group_rx(group_rx_c);

    if( begin == end ) return;
    
    Items::iterator it = get_item( items, parent );
    
    Iterator next(begin); ++next;

    //If element is expander
    if(  expander_rx.exactMatch( *begin ) ){
        it->ex_list.insert( expander_rx.cap(1) );
        //storing config for dynamicaly created items
        it->config_list = std::list<QString>( next, end );
    }
    //If element is grounp
    else if( group_rx.exactMatch( *begin ) ){
        //For ech element in group create items
        BOOST_FOREACH( const QString& sub_str, split( group_rx.cap(1) ) ){
            std::list<typename Iterator::value_type> lst(next, end);
            lst.push_front(sub_str);
            create_items( parent, lst.begin(), lst.end(), items );
        }
    }
    //If element is simple word create item
    else{
        if( static_item ) it->st_list.insert( *begin );
        QStandardItem* item = new QStandardItem( *begin );
        parent->appendRow( item );
        it = get_item( items, item );
        
        create_items( item, next, end, items );
    }
}


rcon::rcon(QWidget* parent, const server_id& id, const server_bookmark& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
    p_->init();

    //TODO move to config file
    QStringList lst;
    lst << "%commands%"
        << "kick %players%"
        << "map %maps%"
        << "forceteam %players% { blue, red }" ;
 

    BOOST_FOREACH ( const QString& cmd_str, lst ) {
        QStringList sp = split( cmd_str );
        create_items( p_->model.invisibleRootItem(), sp.begin(), sp.end(), p_->items );
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

    BOOST_FOREACH( const QByteArray& line, data.split('\n') )
        parse_line( line );

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
    p_->ui.output->setAutoFillBackground( rcon_settings().custom_colors() );
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

    if( p_->current.second == false )
    {
        Q3ColorMap map;
        map[Q3DefaultColor] = p_->colors[rcon_settings::Text];
        print( Simple, q3coloring(line, map) );
    }

    BOOST_FOREACH( ParsersByName::value_type& parser, p_->parsers )
        parser.second->operator()( line );
}


void rcon::print( TextType type, const QString & text )
{
    QString str = QString("<pre><font face=\"Terminus\",\"monospace\">%1</font></pre>").arg(colorize_string(type, text));

    if( !text.isEmpty() )
        p_->ui.output->append( str );
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

void rcon::refresh_expander( const QString& exp )
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
        .arg( p_->options.rcon_password() )
        .arg( p_->current.first.data() );

    p_->waiting = true;
    p_->send_timer.start();
    
    LOG_DEBUG << format( "%1% - send: %2%" ) % p_->id.address().toStdString() % cmd.toStdString();
    p_->socket.write( cmd.toStdString().data() );

    p_->last_send = QTime::currentTime();
}

void erase_item( QStandardItem* item, Items& items ){
    for( int i = 0; i < item->rowCount(); ++i )
        erase_item(item->child( i ), items);

    Items::iterator it = std::find_if( items.begin(), items.end(), bind(&Item::item, _1) == item );
    if( it != items.end() )
        items.erase( it );
}

void rcon::update_item(const Item& item)
{
    LOG_DEBUG << "Updating item " << item.item->text().toStdString();
    QList<QStandardItem*> to_delete;

    //List of all autocompletition elements of item
    Strings full_list;
    //...static items
    std::copy( item.st_list.begin(), item.st_list.end(), std::inserter( full_list, full_list.end() ) );

    //...dynamically expanded items
    BOOST_FOREACH( const QString& exp, item.ex_list )
        std::copy( p_->expanders[exp].begin(), p_->expanders[exp].end(), std::inserter( full_list, full_list.end() ) );

    //checking QStandardItemModel structure
    QStandardItem* child(0);
    Strings::iterator cmd;
    for( int i = 0; i < item.item->rowCount(); ++i ) {
        child = item.item->child( i );

        //Item exixst all ok
        if( cmd = full_list.find( child->text() ), cmd != full_list.end() )
            full_list.erase( cmd );
        // Item does not exist and model item must be removed
        else
            to_delete.push_back( child );
    }

    BOOST_FOREACH( QStandardItem* it, to_delete   )
    {
        LOG_DEBUG << "Removing item" << it->text().toStdString();
        erase_item( it, p_->items );
        p_->model.removeRow( it->row(), item.item->index() );        
    }

    BOOST_FOREACH( const QString& cmd, full_list )
    {
        if ( item.config_list.empty() )
            item.item->appendRow( new QStandardItem( cmd ) );
        else
        {
            std::list<QString> lst( item.config_list.begin(), item.config_list.end() );
            lst.push_front(cmd);
            LOG_DEBUG << "Creating dynamyc item" << cmd.toStdString();
            create_items( item.item, lst.begin(), lst.end(), p_->items, false);
        }
        
    }
    LOG_DEBUG << "Items size:" << p_->items.size();
}




