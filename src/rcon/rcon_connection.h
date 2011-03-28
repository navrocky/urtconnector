
#ifndef URT_RCON_CONNECTION_H
#define URT_RCON_CONNECTION_H

#include <memory>

#include <QObject>


struct server_id;
struct player_info;

class rcon_connection: public QObject{
    Q_OBJECT
public:
    explicit rcon_connection( const server_id& id, const QString& pass, QObject* parent = 0 );
    virtual ~rcon_connection();
    
    void set_server_id( const server_id& id );
    void set_password( const QString& pass );

    void set_auto_update(bool b);
    
   
public Q_SLOTS:
    ///Send command to remote server
    void send_command( const QString& command );

    const QStringList& commands();
    const QStringList& players();
    const QStringList& maps();
    
    void kick_player( const player_info& player );
    void set_map( const QString& map );
    
Q_SIGNALS:
    ///Emited when new data received from remove server
    void received( const QList<QByteArray>& data );

    ///Emited when connection status is changed
    void connection_changed( bool );
    
    void commands_changed( const QStringList& commands );
    void players_changed( const QStringList& players );
    void maps_changed( const QStringList& maps );
    
    void bad_password( const server_id& id );
    
private Q_SLOTS:
    
    void send_internal( const QString& command, bool suppress );
    
    ///New data from server recieved
    void ready_read();
    
    ///handle recevide data
    void process_input( const QList<QByteArray>& data );
    
    ///send queued commands to server
    void process_queue();
    
private:
    void set_state( bool connected );
    void parser_completed( const QString& name );
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


#endif


