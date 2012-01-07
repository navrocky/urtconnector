
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextBrowser>
#include <QTextFrame>
#include <QToolButton>

#define SYSLOG_WRITE_FUNCTIONS

#include "cl/syslog/syslog.h"
#include "common/exception.h"
#include "common/player_info.h"
#include "common/server_info.h"
#include "common/tools.h"
#include "rcon/rcon_connection.h"

#include "geoip/geoip.h"

#include "server_info_manager.h"
#include "common/server_bookmark.h"

const QString player_tag_c = "PLAYER_TAG=\"%1\"";
const QString map_tag_c = "MAP_TAG=\"%1\"";

const char* player_property_c = "player";

SYSLOG_MODULE(server_info_manager);

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
    , rcon_( new rcon_connection( server_id(), "", this ) )
    , browser_( new QTextBrowser(this) )
    , html_colors_( default_colors() )
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
    
    BOOST_FOREACH( Q3ColorMap::value_type& p, html_colors_ )
        p.second = p.second.lighter();

    html_colors_[Q3DefaultColor] = palette().color(QPalette::Text);
    
    connect( this, SIGNAL(kick_player(const player_info&)), rcon_, SLOT(kick_player(const player_info&)) );
    connect( this, SIGNAL(change_map(const QString&)),      rcon_, SLOT(set_map(const QString&)) );
    connect( rcon_, SIGNAL(bad_password(const server_id&)),        this,  SLOT(bad_password(const server_id&)) );
}

server_info_manager::~server_info_manager()
{}

void server_info_manager::set_bookmarks( server_bookmark_list_p bookmarks )
{
    assert( bookmarks.get() );
    
    if( bookmarks_.get() )
        disconnect( bookmarks_.get(), SIGNAL( changed(const server_bookmark&, const server_bookmark&) )
            , this, SLOT( bookmark_changed( const server_bookmark&, const server_bookmark& ) ) );    
        
    bookmarks_ = bookmarks;
        
    if( bookmarks_.get() )
        connect( bookmarks_.get(), SIGNAL( changed(const server_bookmark&, const server_bookmark&) )
            , this, SLOT( bookmark_changed( const server_bookmark&, const server_bookmark& ) ) );
}


void server_info_manager::set_server_info( server_info_p si )
{
    si_ = si;

    if( si_ )
    {
        assert( bookmarks_.get() );
        bm_ = bookmarks_->get( si_->id );
        rcon_->set_server_id( si_->id );
        bookmark_changed( bm_, bm_ );
    }
    else
    {
        widgets_.clear();
        browser_->setHtml( QString() );
    }
}

bool server_info_manager::is_admin() const
{
    return !bm_.rcon_password().isEmpty();
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
//    LOG_HARD << "event:"<<e->type();
    bool ret = QObject::eventFilter(obj, e);
    if( obj == browser_ && ( e->type() == QEvent::Paint || e->type() == QEvent::Resize ) )
    {
//        LOG_HARD << "handling widgets visibility, Paint Event";

        QRect viewport_rect = visible_rect( browser_ );
        
        //QTextBrowser engine does not updates block thats are hidden away from QAbstractScrollArea visible surface
        //So we mannually hide widget when associated block is not in visible area
        QAbstractTextDocumentLayout* tl = browser_->document()->documentLayout();
        BOOST_FOREACH( const widgets_by_block_t::value_type& p, widgets_ )
        {
            const QTextBlock& text_block = p.first;
            const widget_list_t& w = p.second;
//            LOG_HARD << "handling widget favorites. Block:%1 Widget: %2", text_block.blockNumber(), w.front();
            QRect block_rect = tl->blockBoundingRect( text_block ).toRect();
//            LOG_HARD << "block_rect: %1-%2 %3-%4" ,  block_rect.left(), block_rect.top(), block_rect.width(), block_rect.height();
//            LOG_HARD << "viewport: %1-%2 %3-%4" ,  viewport_rect.left(), viewport_rect.top(), viewport_rect.width(), viewport_rect.height();
            std::for_each( w.begin(), w.end(), bind( &QWidget::setVisible, _1, viewport_rect.intersects(block_rect) ) );
//            LOG_HARD << "visibility:"<< viewport_rect.intersects(block_rect);
//            QRect intersected = viewport_rect.intersected( block_rect );
//            LOG_HARD << "intersected: %1-%2 %3-%4" ,  intersected.left(), intersected.top(), intersected.width(), intersected.height();
        }
    }
    return ret;
}

void server_info_manager::bad_password(const server_id& id)
{
    QMessageBox::warning( this, "Bad rcon password", QString( "Bad rcon password for server: %1" ).arg(id.address()) );
}

void server_info_manager::bookmark_changed( const server_bookmark& old_bm, const server_bookmark& new_bm )
{
    if ( !si_ ) return;
    
    //Current server_info bookmark changed OR no bookmark exist for this server
    if( ( old_bm.id() == si_->id ) || ( new_bm.id() == si_->id ) ||
        ( bm_.is_empty() && old_bm.is_empty() && new_bm.is_empty() )  )
    {
        bm_ = new_bm;
        rcon_->set_password( bm_.rcon_password() );
        update();
    }
}

void server_info_manager::update()
{
    widgets_.clear();
    browser_->setHtml( create_html_template(*si_) );
    regenerate_widgets(*si_);
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

    Q3ColorMap colors(html_colors_);
    colors[Q3DefaultColor] = Qt::white;
    
    name = q3coloring(name, colors);

    QString admin_image = ( is_admin() )
        ? QString("<font color=red>ADMIN MODE</font>")
        : QString();
    
    QString body = QString( "<body class=\"body\"><table width=100%>"
                            "<tr><td class=\"serv_header\">%1</td></tr></table>"
                            "%2 %3<hr>%4%5%6</body>" )
        .arg( name )
        .arg( si.id.address() )
        .arg( admin_image )
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
    
    if ( !si.forbidden_gears().empty() )
    {
        QStringList weapons;
        BOOST_FOREACH( Gear g, si.forbidden_gears() ){
            weapons << gear(g);
        }
        
        server_info += tr("<tr class=\"line1\"><td>Forbidden weapons</td><td>%1</td></tr>").arg( weapons.join(", ") );
    }
    
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
    LOG_HARD << "creating players template";
    LOG_EXIT_HARD << "completed";
    const player_info_list& pil = si.players;

    QString players;

    if ( pil.size() > 0 )
    {
        players += tr("<hr>%1 players:<table width=100%>"
                     "<tr class=\"header\"><td>Nick</td><td>Ping</td><td>Score</td></tr>").arg( pil.size() );
        int i = 0;
        foreach (const player_info& pi, pil)
        {
            QString player = QString("<tr class=\"line%1\"><td>%2%3</td><td>%4</td><td>%5</td></tr>")
                .arg( i % 2 + 1 )
                .arg( q3coloring(pi.nick_name(), html_colors_) )
                .arg( player_tag_c.arg( toplainhtml( pi.nick_name() ) ) )
                .arg( pi.ping() )
                .arg( pi.score() );

            LOG_HARD << "adding player " << player.toStdString();
                
            players += player;
            i++;
        }

        players += "</table>";
    }

    LOG_HARD << "success";
    return players;
}

bool soft_equal( const QString& s1, const QString& s2 ){
    return QString::compare( s1,s2, Qt::CaseInsensitive ) == 0;
}

QString make_advanced_info( const server_info::info_t::value_type& info, const server_info& si, const Q3ColorMap& subst){
    
    if( soft_equal(info.first, "admin") && !si.get_info("Email").isEmpty() )
        return QString("<a href=\"mailto:%1&lt;%2&gt;\">%3</a>").arg( toplainhtml( info.second ) ).arg( toplainhtml( si.get_info("Email") ) ).arg( q3coloring(info.second, subst) );
    else if( soft_equal(info.first, "email") )
        return QString("<a href=\"mailto:%1\">%1</a>").arg( toplainhtml( info.second ) );
    else if( ( soft_equal(info.first, "sv_dlurl") || soft_equal(info.first,"_url") ) && info.second.startsWith("http") )
        return QString("<a href=\"%1\">%1</a>").arg( toplainhtml( info.second ) );
    else
        return q3coloring( info.second, subst);
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
                .arg( make_advanced_info(info, si, html_colors_) );
            i++;            
        }
        
        ext_info += "</table>";
    }
    return ext_info;
}

void server_info_manager::regenerate_widgets( const server_info& si )
{
    LOG_DEBUG << "regenerating widgets";
    LOG_EXIT_DEBUG << "widget regenerating completed";
    widgets_.clear();
    regenerate_friends(si);
    regenerate_maps(si);
}

void server_info_manager::regenerate_friends(const server_info& si)
{
    player_info_list plist = si.players;
    LOG_HARD << "players count: " << plist.size();

    QTextCursor cursor( browser_->document() );
    QRegExp friend_rx( player_tag_c.arg("(.*)") );

    while ( cursor = browser_->document()->find( friend_rx, cursor ), !cursor.isNull() ) {
        
        friend_rx.exactMatch( cursor.selectedText() );
        QString player = friend_rx.cap(1);
        
        LOG_HARD << "next player tag finded: "<< player.toStdString();

        player_info_list::iterator pinfo = std::find_if( plist.begin(), plist.end(), bind(&player_info::nick_name, _1) == player );

        // FIXME vlad: this assert crashes on the Chill BOMB[UAA] server, i write this workaround
        //       jerry: some fixes are made. welcome to next test cycle.
        
        if( pinfo == plist.end() ){
            QStringList exception;
            exception << "Can't process playername. Send this error to kinnalru@gmail.com"<<"\n";
            exception << "Server: " << si.name <<"\n";
            exception << "Payer: " << player <<"\n";
            exception << "Payer list: "<<"\n";
            BOOST_FOREACH( const player_info& pi, plist ) {
                exception << "  - " << pi.nick_name() << "\n";
            }
            throw qexception(exception.join(""));
        }
        
        assert( pinfo != plist.end() );//FIXME remove on developing completiion ?

        QWidget* debug_ptr = wrap_widget( create_tool_button(
            QIcon("icons:bookmarks.png"),
            bind( &server_info_manager::add_to_friend, this, *pinfo ) ), cursor );
        
        LOG_HARD<<"wraping player" << pinfo->nick_name().toStdString();
        LOG_HARD<<"wraping favorites pointer" << debug_ptr;
        
        if( is_admin() )
            wrap_widget( create_tool_button(
                QIcon("icons:remove.png"),
                bind( &server_info_manager::kick_player, this, *pinfo ) ), cursor );

        plist.erase( pinfo );
    }
  
    assert( plist.empty() );//FIXME remove on developing completiion ?
}

void server_info_manager::regenerate_maps(const server_info& si)
{
    QTextCursor cursor( browser_->document() );
    QRegExp map_rx( map_tag_c.arg("(.*)") );

    while ( cursor = browser_->document()->find( map_rx, cursor ), !cursor.isNull() ) {

        map_rx.exactMatch( cursor.selectedText() );
        QString map = map_rx.cap(1);

        if( is_admin() )
            wrap_widget( create_map_box( si ), cursor );
        else
            cursor.insertText( map );
    }
}

QToolButton* server_info_manager::create_tool_button(const QIcon& icon, boost::function< void() > action )
{
    QToolButton* button = new QToolButton( browser_->viewport() );
    button->setVisible( browser_->viewport()->isVisible() );
    button->setFixedSize( QSize(22,22) );
    button->setIcon( icon.pixmap(QSize(12,12)) );
    button->setAutoRaise(true);

    connect(
        button, SIGNAL( clicked(bool) ),
        new qt_signal_wrapper( button, action ), SLOT( activate() ) );
    
    return button;
}

QComboBox* server_info_manager::create_map_box(const server_info& si)
{
    QComboBox* combo = new map_combo( browser_->viewport() );
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    combo->setVisible( browser_->viewport()->isVisible() );
    combo->addItem(si.map);
    
    connect( rcon_, SIGNAL(maps_changed(QStringList)), combo, SLOT( set_items(QStringList ) ));
    connect( combo, SIGNAL( before_show() ), rcon_, SLOT( maps() ) );
    
    connect( combo, SIGNAL( currentIndexChanged( const QString& ) ), this, SIGNAL( change_map( const QString& ) ) );
    
    return combo;
}

QWidget* server_info_manager::wrap_widget( QWidget* widget, QTextCursor& cursor ) {
    QTextCharFormat format = cursor.blockCharFormat();
    format.setObjectType( widget_object::WidgetFormat );
    format.setProperty( widget_object::WidgetPtr,    qVariantFromValue<QWidget*>( widget ) );
    format.setProperty( widget_object::VerticalBarPtr, qVariantFromValue<QWidget*>( browser_->verticalScrollBar() ) );
    format.setProperty( widget_object::HorizontalBarPtr, qVariantFromValue<QWidget*>( browser_->horizontalScrollBar() ) );

    cursor.insertText( QString(QChar::ObjectReplacementCharacter), format );

    QObject::connect( cursor.currentFrame(), SIGNAL( destroyed(QObject *) ), widget, SLOT( deleteLater() ) );
    widgets_[ cursor.block() ].push_back( widget );
    return widget;
}

