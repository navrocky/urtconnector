

#ifndef URT_RCON_H
#define URT_RCON_H

#include <memory>

#include <QWidget>

class server_id;
class server_options;

class rcon : public QWidget{
    Q_OBJECT
public:

    enum TextType{
        Command,
        Simple,
        Info,
        Error
    };

    rcon(QWidget* parent, const server_id& id, const server_options& options);
    ~rcon();

public Q_SLOTS:
    ///Send command to server
    void send_command( const QString& command );
    ///New data from server recieved
    void ready_read();

private Q_SLOTS:
    ///used to first initialization rcon-connection
    void connected();
    ///New command to server ready
    void input_enter_pressed();
    ///This function called when answer from server recieved \b OR when timeout for sending reached
    void send_timeout();
    
private:
    ///Connected/disconnected state
    void set_state( bool conencted );
    ///parse recieved data
    bool parse_data( QByteArray ba );

    ///print text with type-specifyed color
    void print( TextType type, const QString& text);
    void print( TextType type, const QByteArray& data);
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


#endif