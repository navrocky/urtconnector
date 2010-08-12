
#include <iostream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/assign/std/map.hpp>

#include <QLabel>
#include <QTimer>
#include <QUdpSocket>
#include <QCompleter>

#include <cl/syslog/syslog.h>

#include "server_id.h"
#include "server_options.h"
#include "tools.h"

#include "ui_rcon.h"
#include "rcon.h"

SYSLOG_MODULE("rcon");

using namespace boost;

const char answer_header_codes[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };
const char rcon_header_codes[10] = {0xff, 0xff, 0xff, 0xff, 'r', 'c', 'o', 'n', 0x20, 0x00};

const QByteArray answer_header_c( answer_header_codes );
const QByteArray rcon_header_c( rcon_header_codes );

typedef boost::function<void ( const QByteArray& )> Handler;

struct rcon::Pimpl{
    Pimpl( const server_id& id, const server_options& options )
        : id(id), options(options)
        , connected(false), waiting(false), skip_command(false)
    {}
    void init() {
        ui.output->setFont( QFont("Terminus") );
        ui.input->setFont( QFont("Terminus") );

        status = new QLabel(0);
        status->setFixedSize( QSize(16, 16) );

        //moving pixmap to Right-Up corner
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget(status);
        vl->addStretch();

        QHBoxLayout* hl = new QHBoxLayout(ui.output);
        hl->addStretch();
        hl->addLayout( vl );

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
    bool skip_command;

    QStringList commands;
    
    QLabel* status;
    QTimer send_timer;

    std::map<rcon_settings::Color, QColor> colors;
    Handler handler;
};

rcon::rcon(QWidget* parent, const server_id& id, const server_options& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
    p_->init();

    p_->ui.input->setCompleter( new completer(p_->ui.input) );

    //UdpSoket always connected, but initialization required
    connect( &p_->socket, SIGNAL( connected() ),   SLOT( connected() ) );
    connect( &p_->socket, SIGNAL( readyRead ()), SLOT( ready_read() ) );

    connect( &p_->send_timer, SIGNAL( timeout() ), SLOT( send_timeout() ) );

    connect( p_->ui.input, SIGNAL( returnPressed() ), SLOT( input_enter_pressed() ));

    update_settings();
    
    p_->socket.connectToHost( p_->id.ip_or_host(), p_->id.port() );
}

rcon::~rcon()
{}

void rcon::send_command( const QString& command )
{
    QString cmd = QString( "%1 %2 %3" )
        .arg( rcon_header_c.data() )
        .arg( p_->options.rcon_password )
        .arg( command );

    p_->ui.input->setReadOnly( true );
    p_->waiting = true;
    p_->send_timer.start();

    LOG_DEBUG << format( "%1% - send: %2%" ) % p_->id.address().toStdString() % cmd.toStdString();
    p_->socket.write( cmd.toStdString().data() );
}


void rcon::ready_read()
{
    p_->send_timer.stop();
    p_->waiting = false;
    send_timeout();

    QByteArray data = p_->socket.readAll();
    LOG_DEBUG << format( "%1% - recieved: %2%" ) % p_->id.address().toStdString() % data.constData();

    BOOST_FOREACH( const QByteArray& line, data.split('\n') ) {
        parse_data( line );
    }
}

void rcon::update_settings()
{
    p_->update_colors();
    QPalette p = p_->ui.output->palette();
    p.setColor( QPalette::Base, p_->colors[rcon_settings::Background] );
    p_->ui.output->setPalette( p );
    p_->ui.output->setAutoFillBackground( !rcon_settings().adaptive_pallete() );

    p.setColor( QPalette::Text, p_->colors[rcon_settings::Command] );
    p_->ui.input->setPalette( p );
    p_->ui.input->setAutoFillBackground( !rcon_settings().adaptive_pallete() );
}


void rcon::connected()
{
    p_->handler = boost::bind( &rcon::get_command, this, _1 );
    p_->commands.clear();
    p_->skip_command = true;
    send_command("cmdlist");
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
    p_->ui.input->setReadOnly( false );
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

void rcon::parse_data( const QByteArray& line )
{
    if( line.startsWith( answer_header_c ) )
    {
        QByteArray command = line.mid( answer_header_c.size(), -1 );
        if( command == "print" && !p_->skip_command )
            p_->handler = boost::bind( &rcon::print, this, Simple, _1 );
    }
    else
    {
        p_->handler( line );
    }
}


void rcon::print(TextType type, const QString & text)
{
    if( !text.isEmpty() )
        p_->ui.output->append( colorize_string(type, text) );
}

void rcon::get_command(const QByteArray& command)
{
    QString text( command.data() );
    if( text.contains( QRegExp("[0-9]+ command") ) )
    {
        p_->skip_command = false;
        p_->ui.input->completer()->setModel( new QStringListModel(p_->commands, p_->ui.input->completer() ) );
    }
    else
    {
        p_->commands << text;
    }
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












