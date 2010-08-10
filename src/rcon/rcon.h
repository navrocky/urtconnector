

#ifndef URT_RCON_H
#define URT_RCON_H

#include <memory>

#include <QWidget>

#include "rcon_settings.h"

class server_id;
class server_options;

class rcon : public QWidget{
    Q_OBJECT
public:

    rcon(QWidget* parent, const server_id& id, const server_options& options);
    ~rcon();

public Q_SLOTS:
    ///Send command to server
    void send_command( const QString& command );
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
    
private:
    ///Connected/disconnected state
    void set_state( bool conencted );
    ///parse recieved data
    void parse_data( const QByteArray& line );

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

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QCoreApplication>
#include <QStringListModel>

#include <iostream>

class my_completer: public QCompleter{
    Q_OBJECT
public:
    my_completer(QLineEdit* l, const QStringList& lst):QCompleter(lst, l), edit(l), items( popup() )
    {
        first = false;
        setCompletionColumn(1);
        connect( this, SIGNAL( activated(const QString&)), SLOT( activated1(const QString&) ) );
    }

private Q_SLOTS:
    void activated1( const QString& text )
    {
        setCompletionColumn(1);
        first = false;
    }
    
    void highlited( const QString& text )
    {
//         edit->setText( edit->text() + text );
    }

    bool edit_event( QEvent* e )
    {
        if( e->type() == QEvent::KeyPress )
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            std::cerr<<"Edit Key:"<<event->key()<<std::endl;
            if( event->key() == Qt::Key_Tab )
            {
                start_completition();
                first = true;
                return true;
            }
            else if(event->key() == Qt::Key_Return)
            {
                std::cerr<<"Edit enteRR!!!!!!"<<std::endl;
            }
            else
            {
                first = false;
            }
        }
        return false;
    }

    bool items_event( QEvent* e)
    {
        if( e->type() == QEvent::KeyPress )
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            std::cerr<<"Items Key:"<<event->key()<<std::endl;
            switch ( event->key() )
            {
                case Qt::Key_Tab:
                    QCoreApplication::postEvent(items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier) );
                    return true;
                    break;
                case Qt::Key_Backtab:
                    QCoreApplication::postEvent(items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier) );
                    return true;
                    break;
                case Qt::Key_Escape:
                    items->hide();
                    setCompletionColumn(1);
                    return true;
                    break;
                case Qt::Key_Return:
                    //QModelIndex m = items->currentIndex();
                    apply_completition( items->currentIndex().data().toString() );
                    setCompletionColumn(1);
                    emit activated( items->currentIndex() );
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

protected:
    virtual bool eventFilter(QObject* o, QEvent* e)
    {
        if( o == edit && edit_event(e) )
            return true;
        else if (o == items && items_event(e) )
            return true;
        else
            return QCompleter::eventFilter(o, e);
    }

    void apply_completition( const QString& str )
    {
        line.replace( start_pos, current_word.size(), str );
        edit->setText( line );
        edit->setCursorPosition( start_pos+ str.size() );
    }

    QString find_equal_begin( const QStringList& lst, const QString& begin)
    {
        std::cerr<<"1"<<std::endl;
        if( lst.isEmpty() || lst[0].size() <= begin.size() )
            return begin;
        std::cerr<<"2"<<std::endl;
        QString b(begin);
        b+=lst[0][begin.size()];
        std::cerr<<"3"<<std::endl;
        bool equal = true;
        for (uint i = 0; i<lst.size(); ++i)
        {
            if( lst[i].size() <= begin.size() )
                return begin;
            
            if ( lst[i][b.size() -1] != b[ b.size() -1 ] )
            {
                equal = false;
                break;
            }
        }
        std::cerr<<"4"<<std::endl;
        if( equal )
            return find_equal_begin(lst, b);
        else
            return begin;
    }

private:

    void init_word()
    {
        line = edit->text();
        start_pos = line.lastIndexOf( " ", edit->cursorPosition()-1 ) +1;
        current_word = line.mid( start_pos, edit->cursorPosition() - start_pos );
        std::cerr<<"Start pos:"<<start_pos<<std::endl;
        std::cerr<<"Current w:"<<current_word.toStdString()<<std::endl;
    }
    
    void start_completition()
    {
        setCompletionColumn(0);

        init_word();

        edit->completer()->setCompletionPrefix( current_word );
        QAbstractItemModel* im = completionModel();
        int rows = im->rowCount();
        std::cerr<<"Rows:"<<rows<<std::endl;
        QStringList lst;
        for(uint i=0; i<rows; ++i)
        {
            lst << im->index(i, 0).data().toString().replace(0, current_word.size(), "");
        }

        QString eb = find_equal_begin( lst, QString() );

        std::cerr<<"equal:"<<(current_word + eb).toStdString()<<std::endl;

        //line.replace( start_pos, current_word.size(), current_word + eb );
        apply_completition( current_word + eb );

        if( first && edit->completer()->completionCount() > 1 )
        {
            edit->completer()->setCompletionPrefix( current_word + eb );
            edit->completer()->complete();
            edit->completer()->popup()->show();

            disconnect( edit->completer(), SIGNAL( highlighted(QString) ), edit, 0 );
            disconnect( edit->completer(), SIGNAL( highlighted(QModelIndex) ), edit, 0 );
            disconnect( edit->completer(), SIGNAL( activated(QString) ), edit, 0 );
            disconnect( edit->completer(), SIGNAL( activated(QModelIndex) ), edit, 0 );
        }
    }

private:
    QString line;
    int start_pos;
    QString current_word;
    bool first;
    QLineEdit* edit;
    QAbstractItemView* items;
};


#endif