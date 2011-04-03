

#ifndef URT_RCON_H
#define URT_RCON_H

#include <memory>

#include <QWidget>

#include "rcon_settings.h"

class QStandardItem;

class server_id;
class server_bookmark;

struct Item;

class rcon : public QWidget{
    Q_OBJECT
public:

    rcon(QWidget* parent, const server_id& id, const server_bookmark& options);
    ~rcon();

public Q_SLOTS:
    ///Send command to server
    void send_command( const QString& command/*, bool supress_print = false*/ );
    
    void received( const QList<QByteArray>& data );
    
   
    ///update settings like colors...
    void update_settings();

private Q_SLOTS:
    ///Connected/disconnected state
    void set_state( bool conencted );
    
    ///New command to server ready
    void input_enter_pressed();
    
    void refresh_expander( const QString& expander );
    
    void refresh_players( const QStringList& players );
    void refresh_commands( const QStringList& commands );
    void refresh_maps( const QStringList& maps );
    
private:
    
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