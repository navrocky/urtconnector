
#ifndef URT_SRV_INFO_MANAGER_H
#define URT_SRV_INFO_MANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QTextObjectInterface>

#include <common/server_info.h>
#include <common/server_bookmark.h>
#include <common/tools.h>

class QTextBrowser;
class QToolButton;
class QComboBox;

class player_info;
class rcon_connection;
class rcon_admin;


class widget_object : public QObject, public QTextObjectInterface
{
     Q_OBJECT
     Q_INTERFACES(QTextObjectInterface)

public:

    enum { WidgetFormat = QTextFormat::UserObject + 1 };
    enum { WidgetPtr = QTextFormat::UserProperty + 1, VerticalBarPtr, HorizontalBarPtr };

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                        const QTextFormat &format);

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                     int posInDocument, const QTextFormat &format);
};

class server_info_manager: public QWidget {
    Q_OBJECT
public:
    server_info_manager( QWidget* parent );
    virtual ~server_info_manager();

    void set_bookmarks( server_bookmark_list_p bookmarks );
    void set_server_info( server_info_p si );

    bool is_admin() const;
    
Q_SIGNALS:
    ///This signal emited when manager wants to add player to favorites 
    void add_to_friend( const player_info& player ) const;
    
    void kick_player( const player_info& player ) const;

    ///This signal emited when manager wants to change map on server 
    void change_map( const QString& map ) const;
    
protected:
    virtual bool eventFilter(QObject* obj, QEvent* e);

private Q_SLOTS:
    void bad_password( const server_id& id );
    void bookmark_changed( const server_bookmark& old_bm, const server_bookmark& new_bm );
    
private:
    enum sorting_mode_t
    {
        sm_nickname,
        sm_ping,
        sm_score
    };

    void update();
    
    QString create_html_template( const server_info& si ) const;
    
    ///Create a part of html string to show server info
    QString make_info( const server_info& si ) const;
    ///Create a part of html string to show status of the server
    QString make_status( const server_info& si ) const;
    ///Create a part of html string to show players
    QString make_players( const server_info& si ) const;
    ///Create a part of html string to show extended server info
    QString make_ext_info( const server_info& si ) const;

    ///Parse internal html document and regenerate all widgets
    void regenerate_widgets( const server_info& si );
    void regenerate_sortings();
    void regenerate_friends( const server_info& si );
    void regenerate_maps( const server_info& si );
    
    ///Cteates and initialize button and connect it to  \a action call
    QToolButton* create_tool_button( const QIcon& icon, boost::function<void()> action );
    
    ///Create combobox with maplist
    QComboBox*  create_map_box( const server_info& si );

    ///Magic function that creates "render" for \p widget at position cursor.
    ///\note ownership of widget is at widget's parent but when textBlock at position \p cursor is deleted widget is deleted \b too
    QWidget* wrap_widget( QWidget* widget, QTextCursor& cursor );

    void set_player_sorting(sorting_mode_t mode);
    
    server_bookmark_list_p bookmarks_;
    QTextBrowser*   browser_;
    
    server_info_p   si_;
    server_bookmark bm_;
    //When widgets are layouted above QTextBrowser we associate them with textblock where they are.
    //We need it to hide widgets when they move out from QAbstractScrollArea visible surface.
    //List used because of multimple widgets can be associated with one QTextBlock
    typedef std::list<QWidget*> widget_list_t;
    typedef std::map<QTextBlock, widget_list_t> widgets_by_block_t;
    widgets_by_block_t widgets_;

    Q3ColorMap html_colors_;
    Q3ColorMap dark_html_colors_;
    
    rcon_connection* rcon_;
    sorting_mode_t sorting_mode_;
};

class map_combo: public QComboBox{
    Q_OBJECT
public:
    map_combo(QWidget* parent)
        :QComboBox(parent){}
        
    ~map_combo(){}
    
    virtual void showPopup(){
        emit before_show();
        QComboBox::showPopup();
    }
    
Q_SIGNALS:
    void before_show();
    
public Q_SLOTS:
    void set_items( const QStringList& items ){
        //Adding items if no such items in list
        foreach( const QString& item, items){
            if( findText(item) == -1 )
                addItem(item);
        }
        
        int cur = currentIndex();
        QStringList to_remove;
        
        //collectiong items text to remove
        for(uint index = 0; index < count(); ++index)
        {
            if( !items.contains( itemText(index) ) && index != cur )
                to_remove << itemText(index);
        }
        
        //removing items
        foreach( const QString& item, to_remove){
            removeItem( findText(item) );
        }
        
        
        //FIXME это для того чтоб уже открытый view обновить
        if( reinterpret_cast<QWidget*>(view())->isVisible() )
        {
            QComboBox::hidePopup();
            QComboBox::showPopup();
        }
    }

};

#endif


