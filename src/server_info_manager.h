
#ifndef URT_SRV_INFO_MANAGER_H
#define URT_SRV_INFO_MANAGER_H

#include <QWidget>
#include <QTextObjectInterface>

#include <common/server_info.h>
#include <common/tools.h>

class QTextBrowser;
class QToolButton;
class QComboBox;

class player_info;

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

    void set_server_info( server_info_p si );

Q_SIGNALS:
    ///This signal emited when manager wants to add player to favorites 
    void add_to_friend( const player_info& player ) const;

    ///This signal emited when manager wants to change map on server 
    void change_map( const QString& map ) const;
    
protected:
    virtual bool eventFilter(QObject* obj, QEvent* e);
    
private Q_SLOTS:
    void friend_added() const;
    
private:

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

    void regenerate_friends( const server_info& si );
    void regenerate_maps( const server_info& si );
    
    ///Cteates and initialize button to handle "add to friends" actions
    QToolButton* create_friend_button( const player_info& player );
    ///Create combobox with maplist
    QComboBox*  create_map_box( const server_info& si );

    ///Magic function that creates "render" for \p widget at position cursor.
    ///\note ownership of widget is at widget's parent but when textBlock at position \p cursor is deleted widget is deleted \b too
    void wrap_widget( QWidget* widget, QTextCursor& cursor );
    
private:
    QTextBrowser*   browser_;
    
    server_info_p   si_;
    //When widgets are layouted above QTextBrowser we associate them with textblock where they are.
    //We need it to hide widgets when they move out from QAbstractScrollArea visible surface.
    //List used because of multimple widgets can be associated with one QTextBlock
    typedef std::list<QWidget*> Widgets;
    typedef std::map<QTextBlock, Widgets> WidgetsByBlock;
    WidgetsByBlock widgets;

    Q3ColorMap html_colors_;
};



#endif


