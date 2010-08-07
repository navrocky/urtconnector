
#include <iostream>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include <QLabel>
#include <QTimer>
#include <QUdpSocket>

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

struct rcon::Pimpl{
    Pimpl( const server_id& id, const server_options& options )
        : id(id), options(options)
        , connected(false), waiting(false)
    {}
    void init()
    {
        ui.output->setFont( QFont("Terminus") );

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
    
    Ui_rcon ui;
    server_id id;
    server_options options;
    QUdpSocket socket;
    bool connected;
    bool waiting;
    
    QLabel* status;
    QTimer send_timer;
};

rcon::rcon(QWidget* parent, const server_id& id, const server_options& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
        p_->ui.output->setFont( QFont("Terminus") );

        p_->status = new QLabel(0);
        p_->status->setFixedSize( QSize(16, 16) );

        //moving pixmap to Right-Up corner
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget(p_->status);
        vl->addStretch();

        QHBoxLayout* hl = new QHBoxLayout(p_->ui.output);
        hl->addStretch();
        hl->addLayout( vl );

        p_->send_timer.setInterval( 2000 );
        p_->send_timer.setSingleShot( true );

    //UdpSoket always connected, but initialization required
    connect( &p_->socket, SIGNAL( connected() ),   SLOT( connected() ) );
    connect( &p_->socket, SIGNAL( readyRead ()), SLOT( ready_read() ) );

    connect( &p_->send_timer, SIGNAL( timeout() ), SLOT( send_timeout() ) );

    connect( p_->ui.input, SIGNAL( returnPressed() ), SLOT( input_enter_pressed() ));

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
    
    if ( !parse_data( data ) )
    {
        print( Error, tr("Unknown aswer type:") );
        print( Error, data );
    }
}

void rcon::connected()
{
    send_command("ping localhost");
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

bool rcon::parse_data( QByteArray ba )
{
    bool processed = false;
    if( ba.startsWith( answer_header_c ) )
    {
        ba.remove(0, answer_header_c.size() );
        QByteArray command = ba.left( ba.indexOf('\n') );
        ba.remove(0, command.size() );
        if( command == "print" )
        {
            print( Simple, ba );
            processed = true;
        }
    }
    return processed;
}

QString formatted_string( rcon::TextType type, const QString& text )
{
    switch (type)
    {
        case rcon::Command: return colorize( text, QColor(Qt::yellow).lighter() );
        //hack to skip colorizing quake-default color(white)
        case rcon::Simple:  return q3coloring(text, "7");
        case rcon::Info:    return colorize( text, QColor(Qt::cyan).lighter() );
        case rcon::Error:   return colorize( text, QColor(Qt::red).lighter() );
        default:
            return text;
    }
}

void rcon::print(TextType type, const QString & text)
{
    if( !text.isEmpty() )
    {
        p_->ui.output->append( formatted_string(type, text) );
    }
}

void rcon::print(rcon::TextType type, const QByteArray& data)
{
    BOOST_FOREACH( const QByteArray& line, data.split('\n') ) {
        print( type, QString(line.constData()) );
    }
}











