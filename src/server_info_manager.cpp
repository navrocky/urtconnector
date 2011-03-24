
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <QToolButton>
#include <QScrollBar>
#include <QTextFrame>

#include "common/exception.h"
#include "common/player_info.h"
#include "common/server_info.h"
#include "common/tools.h"

#include "geoip/geoip.h"

#include "server_info_manager.h"

const QString friend_tag_c = "FRIEND_TAG=\"%1\"";

const char* player_property_c = "player";

using namespace boost;

QSizeF widget_object::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    QWidget* widget = qVariantValue<QWidget*>( format.property( widget_object::WidgetPtr ) );

    assert( widget );//FIXME replace on exception on developing completiion ?
    
    return widget->size();
}

void widget_object::drawObject(QPainter* painter, const QRectF& rect, QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    QWidget* widget = qVariantValue<QWidget*>( format.property( widget_object::WidgetPtr ) );
    QWidget* scroll = qVariantValue<QWidget*>( format.property( widget_object::ScrollBarPtr ) );

    assert( widget && scroll );//FIXME replace on exception on developing completiion ?
    
    int ypos = static_cast<QScrollBar*>( scroll )->value();

    widget->setGeometry( rect.toRect().adjusted( 0, -ypos, 0, -ypos ) );
}


QString get_css( const QPalette& palette )
{
    //generic window color
    QString window = palette.color(QPalette::Window).name();
    //background color for text entry widgets
    QString base = palette.color(QPalette::Base).name();
    //alternate background color in views with alternating row colors
    QString alternate = palette.color(QPalette::AlternateBase).name();
    //text color
    QString text = palette.color(QPalette::Text).name();

    static const QString css(
        "<style> "
        ".header{background-color: %1;}"
        ".props{margin-left:20px; margin-right:0px; background-color: %2; width: 100%; border-width:0px;}"
        ".line1{background-color: %3;}"
        ".line2{background-color: %4;}"
        ".img1{margin-right: 10px;}"
        ".serv_header{background-color: black; color:white;"
        "padding:5px 10px 5px 10px;font: bold 12pt; font-family:monospace;}"
        ".body{ color: %5;}"
        "</style>");
    
    return css.arg(window).arg(window).arg(base).arg(alternate).arg(text);
}

server_info_manager::server_info_manager(QWidget* parent): QTextBrowser(parent)
{}

server_info_manager::~server_info_manager()
{}

void server_info_manager::set_server_info(const server_info& si)
{
    setHtml( create_html_template(si) );
    regenerate_widgets(si);
}

void server_info_manager::add_friend() const
{
    QToolButton* tb = qobject_cast<QToolButton*>( sender() );
    assert(tb);//FIXME replace on exception on developing completiion ?
    
    QString name = tb->property( player_property_c ).value<player_info>().nick_name();
    if( name.isEmpty() ) throw qexception( tr( "Player name is empty" ) );

    emit add_to_friend( name );
}

QString server_info_manager::create_html_template(const server_info& si) const
{
    QString html, players;

    const player_info_list& pil = si.players;

    if (pil.size() > 0)
    {
        players = tr("<hr>%1 players:<table width=100%>"
                     "<tr class=\"header\"><td>Nick</td><td>Ping</td><td>Score</td></tr>").arg(pil.size());
        int i = 0;
        foreach (const player_info& pi, pil)
        {
            players += QString("<tr class=\"line%1\"><td>%2%3</td><td>%4</td><td>%5</td></tr>")
                .arg( i % 2 + 1 )
                .arg( Qt::escape(pi.nick_name()) )
                .arg( friend_tag_c.arg(Qt::escape(pi.nick_name())) )
                .arg( pi.ping() )
                .arg( pi.score() );
            i++;
        }
        players += "</table>";
    }

    QString name = Qt::escape(si.get_info("sv_hostname"));
    if (name.isEmpty())
        name = si.get_info("hostname");
    if (name.isEmpty())
        name = si.name;
    if ( name.isEmpty() )
        name = tr("* Unnamed *");

    static Q3ColorMap html_colors;

    if( html_colors.empty() )
    {
        html_colors = default_colors();
        BOOST_FOREACH( Q3ColorMap::value_type& p, html_colors )
            p.second = p.second.lighter();
    }

    name = q3coloring(name, html_colors);

    QString status_str = make_status(si);
    QString serv_info;

    QString country_flag;
    if( !si.country_code.isEmpty() )
         country_flag = QString("<img class=\"img1\" src=\"%1\">")
                 .arg(geoip::get_flag_filename_by_country(si.country_code));

    serv_info = tr( "<table width=100% class=\"props\">"
                    "<tr class=\"line1\"><td>Status</td><td>%1</td></tr>"
                    "<tr class=\"line2\"><td>Game mode</td><td>%2</td></tr>"
                    "<tr class=\"line1\"><td>Map</td><td>%3</td></tr>"
                    "<tr class=\"line2\"><td>Ping</td><td>%4</td></tr>"
                    "<tr class=\"line1\"><td>Country</td><td>%5 %6</td></tr>"
                    "<tr class=\"line2\"><td>Public slots</td><td>%7</td></tr>"
                    "<tr class=\"line1\"><td>Total slots</td><td>%8</td></tr>"
                    "</table>"
                    )
            .arg(status_str).arg(si.mode_name()).arg(si.map).arg(si.ping)
            .arg(country_flag).arg(si.country).arg(si.max_player_count - si.private_slots())
            .arg(si.max_player_count);

    QString ext_info;
    if (si.info.size() > 0)
    {
        ext_info = tr(  "<hr>Extended info:"
                        "<table width=100% class=\"props\">"
                        "<tr class=\"header\"><td>Key</td><td>Value</td></tr>");
       
        int i = 0;
        for (server_info::info_t::const_iterator it = si.info.begin();
                it != si.info.end(); it++)
        {
            ext_info += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td></tr>")
                        .arg(i % 2 + 1).arg(it->first).arg(Qt::escape(it->second));
            i++;
        }
        ext_info += "</table>";
    }

    html = QString("<html><head>%1</head><body class=\"body\"><table width=100%><tr><td class=\"serv_header\">%2"
            "</td></tr></table>%3<hr>%4%5%6</body></html>")
            .arg(get_css( palette() )).arg(name).arg(si.id.address()).arg(serv_info).arg(players)
            .arg(ext_info);
    return html;
}


void server_info_manager::regenerate_widgets( const server_info& si )
{
    player_info_list plist = si.players;
    
    QTextCursor cursor;
    QRegExp friend_rx( friend_tag_c.arg("(.*)") );

    while ( cursor = document()->find( friend_rx ), !cursor.isNull() ){

        friend_rx.exactMatch( cursor.selectedText() );
        QString player = friend_rx.cap(1);

        player_info_list::iterator pinfo = std::find_if( plist.begin(), plist.end(), bind(&player_info::nick_name, _1) == player );
        
        assert( pinfo != plist.end() );//FIXME replace on exception on developing completiion ?

        QToolButton* button = create_friend_button( *pinfo, cursor.currentFrame() );

        //Creating specific char-format for widget geometry handling
        QTextCharFormat format;
        format.setObjectType( widget_object::WidgetFormat );

        format.setProperty( widget_object::WidgetPtr,    qVariantFromValue<QWidget*>( button ) );
        format.setProperty( widget_object::ScrollBarPtr, qVariantFromValue<QWidget*>( verticalScrollBar() ) );

        cursor.insertText( QString(QChar::ObjectReplacementCharacter), format );

        plist.erase( pinfo );
    }

    assert( plist.empty() );//FIXME replace on exception on developing completiion ?
}

QToolButton* server_info_manager::create_friend_button( const player_info& player, QObject* dispatcher )
{
    QToolButton* button = new QToolButton( viewport() );
    button->setVisible( viewport()->isVisible() );
    button->setFixedSize( QSize(22,22) );
    button->setIcon( QIcon("icons:bookmarks.png").pixmap(QSize(12,12)) );
    button->setAutoRaise(true);
    button->setProperty( player_property_c, qVariantFromValue( player ) );

    connect( button, SIGNAL( clicked(bool) ), this, SLOT( add_friend() ) );
    connect( dispatcher, SIGNAL( destroyed(QObject *) ), button, SLOT( deleteLater() ) );

    return button;
}

namespace {
    enum Status{ Illegal = 0, Updating = 2, None = 4, Online = 8, Password = 16, Offline = 32 };
}

QString server_info_manager::make_status(const server_info& si) const
{
    static const std::map<Status, QString> status = assign::map_list_of
        ( Updating , "<img class=\"img1\" src=\"icons:status-update.png\"> Updating " )
        ( None     , "<img class=\"img1\" src=\"icons:status-none.png\"> Unknown "    )
        ( Online   , "<img class=\"img1\" src=\"icons:status-online.png\"> Online "   )
        ( Password , "<img class=\"img1\" src=\"icons:status-passwd.png\"> Online "   )
        ( Offline  , "<img class=\"img1\" src=\"icons:status-offline.png\"> Offline " );
    
    Status st(Illegal);

    if ( si.updating )
        st = Updating;
    else if( si.status == server_info::s_none )
        st = None;
    else if( si.status == server_info::s_up && si.is_password_needed() )
        st = Password;
    else if( si.status == server_info::s_up && !si.is_password_needed() )
        st = Online;
    else if( si.status == server_info::s_down )
        st = Offline;

    assert( st != Illegal );//FIXME replace on exception on developing completiion ?
    
    QString status_str = tr( qPrintable(status.at(st) ) );
    
    if ( si.is_full() && ( st & (Password | Online) ) )
        status_str += tr("Full");
    
    return status_str;
}



