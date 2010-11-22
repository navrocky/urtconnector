
#include <iostream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/assign/list_inserter.hpp>

#include <QLabel>
#include <QTimer>
#include <QUdpSocket>
#include <QCompleter>
#include <QCompleter>
#include <QTableView>
#include <QProxyModel>
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


SYSLOG_MODULE("rcon");

using namespace boost;
using namespace boost::assign;

struct base_parser;

const char answer_header_codes[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };
const char rcon_header_codes[10] = {0xff, 0xff, 0xff, 0xff, 'r', 'c', 'o', 'n', 0x20, 0x00};

const QByteArray answer_header_c( answer_header_codes );
const QByteArray rcon_header_c( rcon_header_codes );

const QString cmd_parser_c = "cmd_parser";
const QString plr_parser_c = "plr_parser";
const QString map_parser_c = "map_parser";


typedef std::pair<QString, bool>        CommandOpt;
typedef QList<CommandOpt>               CommandQueue;

typedef boost::shared_ptr<base_parser>  Parser;
typedef std::map<QString, Parser>       ParsersByName;


struct base_parser{
    typedef boost::function<void ()> Action;
    
    base_parser():enabled_(false){}
    
    virtual ~base_parser(){}

    void enable(){ enabled_ = true; if( start ) start(); };
    void disable(){ enabled_ = false; if( stop ) stop(); };
    bool is_enabled() const { return enabled_; }

    virtual void operator()( const QByteArray& line ) = 0;

    Action start; Action stop;
private:
    bool enabled_;
};


struct command_list_parser: base_parser{
    QStringList& commands;
    const QRegExp cmdlist_end;
    
    command_list_parser( QStringList& c ) :commands(c), cmdlist_end("(\\d+) commands")
    {}

    virtual void operator()(const QByteArray& line){
        if( !is_enabled() ) return;

        if( cmdlist_end.exactMatch(line) )
            disable();
        else
            commands << line;
    }
};


struct player_list_parser: base_parser{
    QStringList& players;
    const QRegExp player;
    
    player_list_parser( QStringList& p ) :players(p), player("\\s+(\\d+):\\s+\\[(.*)\\]")
    {}

    virtual void operator()(const QByteArray& line){

        if( line == "Current players:" )
            enable();
        else if( line == "End current player list." )
            disable();
        else if( is_enabled() && player.exactMatch(line) )
            players << player.cap(2);
    }
};


struct map_list_parser: base_parser{
    QStringList& maps;
    const QRegExp map;
    const QRegExp maplist_end;

    map_list_parser( QStringList& m ) :maps(m), map("maps/(.*).bsp"), maplist_end( "(\\d+) files listed" )
    {}

    virtual void operator()(const QByteArray& line){

        if( line == "---------------" )
            enable();
        else if( maplist_end.exactMatch(line) )
            disable();
        else if( is_enabled() && map.exactMatch(line) )
            maps << map.cap(1);
    }
};



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

    QStringList commands;
    QStringList players;
    QStringList maps;

    //last sended command
    CommandOpt      current;
    CommandQueue    queue;
    
    QTimer          queue_timer;
    QTimer          send_timer;

    //TODO port to cl::timer
    QTime           last_send;

    QStandardItemModel  model;
    QStandardItem*      kick_item;
    QStandardItem*      map_item;

    ParsersByName parsers;
};

rcon::rcon(QWidget* parent, const server_id& id, const server_options& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
    p_->init();

   
    p_->kick_item = new QStandardItem("kick");
    p_->map_item  = new QStandardItem("map");
    
    p_->model.invisibleRootItem()->appendRow( p_->kick_item );
    p_->model.invisibleRootItem()->appendRow( p_->map_item );


    insert( p_->parsers )
        ( cmd_parser_c  , Parser( new command_list_parser( p_->commands ) ) )
        ( plr_parser_c  , Parser( new player_list_parser ( p_->players ) )  )
        ( map_parser_c  , Parser( new map_list_parser    ( p_->maps ) )     )
    ;

    p_->parsers[cmd_parser_c]->start = bind( &QStringList::clear, ref( p_->commands ) );
    p_->parsers[cmd_parser_c]->stop  = bind( &rcon::update_model, this, p_->model.invisibleRootItem(), ref( p_->commands ) );

    p_->parsers[plr_parser_c]->start = bind( &QStringList::clear, ref( p_->players ) );
    p_->parsers[plr_parser_c]->stop  = bind( &rcon::update_model, this, p_->kick_item, ref( p_->players ) );

    p_->parsers[map_parser_c]->start = bind( &QStringList::clear, ref( p_->maps ) );
    p_->parsers[map_parser_c]->stop  = bind( &rcon::update_model, this, p_->map_item, ref( p_->maps ) );

    
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

//     QTreeView* v = new QTreeView(0);
//     v->show();
//     v->setModel( &p_->model );    
}

rcon::~rcon()
{}

void rcon::send_command( const QString& command, bool supress )
{
    if( command.isEmpty() )
        return;
    
    p_->queue.push_back( CommandOpt(command, supress) );
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

    p_->parsers[cmd_parser_c]->enable();
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

void rcon::process_queue()
{
    if( p_->queue.isEmpty() )
        return;

    p_->current = p_->queue.front();
    p_->queue.pop_front();
   
    QString cmd = QString( "%1 %2 %3" )
        .arg( rcon_header_c.data() )
        .arg( p_->options.rcon_password )
        .arg( p_->current.first );

    p_->waiting = true;
    p_->send_timer.start();
    
    LOG_DEBUG << format( "%1% - send: %2%" ) % p_->id.address().toStdString() % cmd.toStdString();
    p_->socket.write( cmd.toStdString().data() );

    p_->last_send = QTime::currentTime();
}


void rcon::update_model( QStandardItem* parent, const QStringList& childs )
{
    QList<QStandardItem*> to_delete;
    QStringList items( childs );

    QStandardItem* child(0);
    QStringList::iterator cmd;
    for( int i = 0; i < parent->rowCount(); ++i ) {
        child = parent->child( i );
        
        if( cmd = std::find( items.begin(), items.end(), child->text() ), cmd != items.end() )
            items.erase( cmd );
        else
            to_delete.push_back( child );
    }

    BOOST_FOREACH( const QStandardItem* it, to_delete   )
        p_->model.removeRow( it->row(), parent->index() );
    
    BOOST_FOREACH( const QString& cmd, items )
        parent->appendRow( new QStandardItem( cmd ) );
}



