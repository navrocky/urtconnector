

#ifndef URT_RCON_H
#define URT_RCON_H

#include <memory>

#include <QWidget>

#include "rcon_settings.h"

class QStandardItem;

class server_id;
class server_options;

struct Item;

class rcon : public QWidget{
    Q_OBJECT
public:

    rcon(QWidget* parent, const server_id& id, const server_options& options);
    ~rcon();

public Q_SLOTS:
    ///Send command to server
    void send_command( const QString& command, bool supress_print = false );
    
    ///New data from server recieved
    void ready_read();
    
    ///update settings like colors...
    void update_settings();

private Q_SLOTS:
    ///used to first initialization rcon-connection
    void connected();
    
    ///New command to server ready
    void input_enter_pressed();
    
    ///This function called when answer from server recieved \b OR when timeout for sending reached
    void send_timeout();

    void refresh_players();
    
    void refresh_maps();

    void process_queue();

    void refresh_expander( const std::string& expander );
    
private:
    ///Connected/disconnected state
    void set_state( bool conencted );
    
    ///parse recieved data
    void parse_line( const QByteArray& line );

    ///Update autocompletition of item
    void update_item( const Item& item);
    
    enum TextType{
        Command,
        Simple,
        Info,
        Error
    };
    
    ///print text with type-specifyed color
    void print( TextType type, const QString& text);

    QString colorize_string( rcon::TextType type,  const QString& str ) const;
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


#endif