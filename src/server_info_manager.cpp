
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <QTextBrowser>
#include <QToolButton>
#include <QComboBox>
#include <QScrollBar>
#include <QTextFrame>
#include <QBoxLayout>

#include "common/exception.h"
#include "common/player_info.h"
#include "common/server_info.h"
#include "common/tools.h"

#include "geoip/geoip.h"

#include "server_info_manager.h"

const QString friend_tag_c = "FRIEND_TAG=\"%1\"";
const QString map_tag_c = "MAP_TAG=\"%1\"";

const char* player_property_c = "player";

using namespace boost;

QSizeF widget_object::intrinsicSize(QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    QWidget* widget = qVariantValue<QWidget*>( format.property( widget_object::WidgetPtr ) );
    assert( widget );
    
    return widget->size();
}

void widget_object::drawObject(QPainter* painter, const QRectF& rect, QTextDocument* doc, int posInDocument, const QTextFormat& format)
{
    QWidget* widget  = format.property( widget_object::WidgetPtr ).value<QWidget*>();
    QWidget* vscroll = format.property( widget_object::VerticalBarPtr ).value<QWidget*>();
    QWidget* hscroll = format.property( widget_object::HorizontalBarPtr ).value<QWidget*>();
    assert( widget && vscroll && hscroll );
    
    int ypos = static_cast<QScrollBar*>( vscroll )->value();
    int xpos = static_cast<QScrollBar*>( hscroll )->value();

    widget->setGeometry( rect.toRect().adjusted( -xpos, -ypos, -xpos, -ypos ) );
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

server_info_manager::server_info_manager( QWidget* parent )
    : QWidget( parent )
    , browser_( new QTextBrowser(this) )
{
    //Registering our-handler to make QTextBrowser widget-embeddable
    QObject *wInterface = new widget_object;
    browser_->document()->documentLayout()->registerHandler(widget_object::WidgetFormat, wInterface);

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget( browser_ );

    browser_->installEventFilter(this);
    browser_->setOpenLinks(true);
    browser_->setOpenExternalLinks(true);
}

server_info_manager::~server_info_manager()
{}

void server_info_manager::set_server_info( server_info_p si )
{
    si_ = si;

    widgets.clear();
    
    if( si_ )
    {
        browser_->setHtml( create_html_template(*si_) );
        regenerate_widgets(*si_);
    }
    else
    {
        browser_->setHtml( QString() );
    }
}

///Visible part of scroll area
QRect visible_rect( const QAbstractScrollArea* a ){
    return QRect(
      	a->horizontalScrollBar()->value(),
        a->verticalScrollBar()->value(),
        a->viewport()->width(),
        a->viewport()->height()
    );
}

bool server_info_manager::eventFilter(QObject* obj, QEvent* e)
{
    bool ret = QObject::eventFilter(obj, e);
    
    if( obj == browser_ && e->type() == QEvent::Paint)
    {
        //QTextBrowser engine does not updates block thas are hidden away from QAbstractScrollArea visible surface
        //So we mannually hide widget when associated block is not in visible area
        BOOST_FOREACH( const WidgetsByBlock::value_type& p, widgets ){
            QRect block_rect = browser_->document()->documentLayout()->blockBoundingRect( p.first ).toRect();
            QRect viewport_rect = visible_rect( browser_ );

            std::for_each( p.second.begin(), p.second.end(), bind( &QWidget::setVisible, _1, viewport_rect.intersects(block_rect) ) );
        }
    }
    return ret;
}

void server_info_manager::friend_added() const
{
    QToolButton* tb = qobject_cast<QToolButton*>( sender() );
    assert(tb);//FIXME replace on exception on developing completiion ?

    if( tb->property( player_property_c ).canConvert<player_info>() )
        throw qexception( tr( "Can't extract player info" ) );

    emit add_to_friend( tb->property( player_property_c ).value<player_info>() );
}

QString server_info_manager::create_html_template(const server_info& si) const
{
    QString name = si.get_info("sv_hostname");
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

    QString body = QString( "<body class=\"body\"><table width=100%>"
                            "<tr><td class=\"serv_header\">%1</td></tr></table>"
                            "%2<hr>%3%4%5</body>" )
        .arg( name )
        .arg( si.id.address() )
        .arg( make_info(si) )
        .arg( make_players(si) )
        .arg( make_ext_info(si) );

    return QString( "<html><head>%1</head>%2</html>").arg( get_css( palette() ) ).arg(body);
}

namespace {
    enum Status{ Illegal = 0, Updating = 2, None = 4, Online = 8, Password = 16, Offline = 32 };
}

QString server_info_manager::make_info(const server_info& si) const
{
    QString country_flag = ( si.country_code.isEmpty() )
        ? QString()
        : QString("<img class=\"img1\" src=\"%1\">").arg(geoip::get_flag_filename_by_country(si.country_code));

    QString server_info;
    server_info += "<table width=100% class=\"props\">";
    
    server_info += tr("<tr class=\"line1\"><td>Status</td><td>%1</td></tr>").arg( make_status(si) );
    server_info += tr("<tr class=\"line2\"><td>Game mode</td><td>%1</td></tr>").arg( si.mode_name() );
    server_info += tr("<tr class=\"line1\"><td>Map</td><td>%1</td></tr>").arg( map_tag_c.arg(Qt::escape(si.map)) );
    server_info += tr("<tr class=\"line2\"><td>Ping</td><td>%1</td></tr>").arg(si.ping);
    server_info += tr("<tr class=\"line1\"><td>Country</td><td>%1 %2</td></tr>").arg(country_flag).arg(si.country);
    server_info += tr("<tr class=\"line2\"><td>Public slots</td><td>%1</td></tr>").arg(si.max_player_count - si.private_slots());
    server_info += tr("<tr class=\"line1\"><td>Total slots</td><td>%1</td></tr>").arg(si.max_player_count);
    
    server_info += "</table>";

    return server_info;
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

QString server_info_manager::make_players(const server_info& si) const
{
    const player_info_list& pil = si.players;

    QString players;

    if ( pil.size() > 0 )
    {
        players += tr("<hr>%1 players:<table width=100%>"
                     "<tr class=\"header\"><td>Nick</td><td>Ping</td><td>Score</td></tr>").arg( pil.size() );
        int i = 0;
        foreach (const player_info& pi, pil)
        {
            players += QString("<tr class=\"line%1\"><td>%2%3</td><td>%4</td><td>%5</td></tr>")
                .arg( i % 2 + 1 )
                .arg( q3coloring(pi.nick_name()) )
                .arg( friend_tag_c.arg(pi.nick_name()) )
                .arg( pi.ping() )
                .arg( pi.score() );
            i++;
        }

        players += "</table>";
    }

    return players;
}

QString make_advanced_info( const server_info::info_t::value_type& info, const server_info& si ){
    
    if( info.first == "Admin" )
        return QString("<a href=\"mailto:%1;%2;\">%1</a>").arg( toplainhtml( info.second ) ).arg( toplainhtml( si.get_info("Email") ) );
    else if( info.first == "Email" )
        return QString("<a href=\"mailto:%1\">%1</a>").arg( toplainhtml( info.second ) );
    else if( info.first == "sv_dlURL" && info.second.startsWith("http") )
        return QString("<a href=\"%1\">%1</a>").arg( toplainhtml( info.second ) );
    else
        return q3coloring( info.second );
}

QString server_info_manager::make_ext_info(const server_info& si) const
{
    QString ext_info;
    if ( si.info.size() > 0 )
    {
        ext_info += tr( "<hr>Extended info:"
                        "<table width=100% class=\"props\">"
                        "<tr class=\"header\"><td>Key</td><td>Value</td></tr>");
        int i = 0;
        BOOST_FOREACH( const server_info::info_t::value_type& info, si.info ) {
            ext_info += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td></tr>")
                .arg( i % 2 + 1 )
                .arg( toplainhtml( info.first ) )
                .arg( make_advanced_info(info, si) );
            i++;            
        }
        
        ext_info += "</table>";
    }
    return ext_info;
}

void server_info_manager::regenerate_widgets( const server_info& si )
{
    widgets.clear();
    regenerate_friends(si);
    regenerate_maps(si);
}

void server_info_manager::regenerate_friends(const server_info& si)
{
    player_info_list plist = si.players;

    QTextCursor cursor( browser_->document() );
    QRegExp friend_rx( friend_tag_c.arg("(.*)") );

    while ( cursor = browser_->document()->find( friend_rx, cursor ), !cursor.isNull() ) {

        friend_rx.exactMatch( cursor.selectedText() );
        QString player = friend_rx.cap(1);

        player_info_list::iterator pinfo = std::find_if( plist.begin(), plist.end(), bind(&player_info::nick_name, _1) == player );

        // FIXME vlad: this assert crashes on the Chill BOMB[UAA] server, i write this workaround
        if (pinfo == plist.end())
            continue;
        
        assert( pinfo != plist.end() );//FIXME remove on developing completiion ?

        wrap_widget( create_friend_button( *pinfo ), cursor );

        plist.erase( pinfo );
    }
  
// this is an errorneous player list from html-browser on ChillBombServer
/*    
/29/...[Mursel]￼
1664￼
Krycha89FRIEND_TAG="Krycha89"
ACAB￼
LowPing:(((￼
..:: JoinT::..￼
Possest￼
*'GWRR'*Sr.L3!￼
xXTheKiLLXx￼
PongoPygmaeus￼
lr￼
MadafakaFRIEND_TAG="Madafaka"
Wuszu@￼
Saucisson￼
pulpfictlags￼
*/

// FIXME vlad: this code failed too
//    assert( plist.empty() );//FIXME remove on developing completiion ?
}

void server_info_manager::regenerate_maps(const server_info& si)
{
    QTextCursor cursor( browser_->document() );
    QRegExp map_rx( map_tag_c.arg("(.*)") );

    while ( cursor = browser_->document()->find( map_rx, cursor ), !cursor.isNull() ) {

        map_rx.exactMatch( cursor.selectedText() );
        QString map = map_rx.cap(1);

        wrap_widget( create_map_box( si ), cursor );
    }
}

QToolButton* server_info_manager::create_friend_button( const player_info& player )
{
    QToolButton* button = new QToolButton( browser_->viewport() );
    button->setVisible( browser_->viewport()->isVisible() );
    button->setFixedSize( QSize(22,22) );
    button->setIcon( QIcon("icons:bookmarks.png").pixmap(QSize(12,12)) );
    button->setAutoRaise(true);
    button->setProperty( player_property_c, qVariantFromValue( player ) );

    connect( button, SIGNAL( clicked(bool) ), this, SLOT( friend_added() ) );
    return button;
}

QComboBox* server_info_manager::create_map_box(const server_info& si)
{
    QComboBox* combo = new QComboBox( browser_->viewport() );
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    combo->setVisible( browser_->viewport()->isVisible() );
    combo->addItem(si.map);
    connect( combo, SIGNAL( currentIndexChanged( const QString& ) ), this, SIGNAL( change_map( const QString& ) ) );
    return combo;
}

void server_info_manager::wrap_widget( QWidget* widget, QTextCursor& cursor ) {
    QTextCharFormat format = cursor.blockCharFormat();
    format.setObjectType( widget_object::WidgetFormat );
    format.setProperty( widget_object::WidgetPtr,    qVariantFromValue<QWidget*>( widget ) );
    format.setProperty( widget_object::VerticalBarPtr, qVariantFromValue<QWidget*>( browser_->verticalScrollBar() ) );
    format.setProperty( widget_object::HorizontalBarPtr, qVariantFromValue<QWidget*>( browser_->horizontalScrollBar() ) );

    cursor.insertText( QString(QChar::ObjectReplacementCharacter), format );

    QObject::connect( cursor.currentFrame(), SIGNAL( destroyed(QObject *) ), widget, SLOT( deleteLater() ) );
    widgets[ cursor.block() ].push_back( widget );
}

