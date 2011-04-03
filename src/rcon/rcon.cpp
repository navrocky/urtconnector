
#include <set>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <QLabel>
#include <QStandardItemModel>

#include <cl/syslog/syslog.h>

#include "common/server_bookmark.h"
#include "common/tools.h"

#include "rcon_connection.h"
#include "rcon_completer.h"

#include "ui_rcon.h"
#include "rcon.h"

SYSLOG_MODULE(rcon)

using namespace boost;
using namespace boost::assign;

const QString exp_rx_c( "%(\\w+)%" );
const QString group_rx_c( "\\{(.*)\\}" );
const QString word_rx_c( "\\w+" );

const QString config_rx_c( QString("%1|%2|%3").arg(exp_rx_c).arg(group_rx_c).arg(word_rx_c) );

///List of unique strings
typedef std::set<QString>           Strings;
typedef std::map<QString, Strings>  ExpandersByName;


/**
 * Expandable model:
 * 
 * QCompleter uses QStandartItemModel to implement tree of variants to autocompletition.
 * 
 * RCon implements dynamic completition on top of QCompleter:
 * 
 * 1. when some dynamic items are upadted we must update QCompleter model
 * 2. rcon uses a simple syntax of text in standart model to do flexible configuration of autocompletition:
 *      if text match this syntax - item is handled not as usually
 *      
 * syntax:
 * 
 * word     = text
 * expander = '%' text '%'
 * element  = word | expander
 * group    = '{' (element | group)+ '}'
 * token    = group | element
 * 
 * line     = token+
 * 
 * Each token represents a level in autocompletition hierarchy
 * Each expander represents an array of words.
 * 
 * Examples:
 * 1. "%commands%"     - first level list of words, that would be autocompleted when typing starts
 * 2. "kick jerry"     - "kick" is added to first level, "jerry" to second level AFTER kick
 * 3. "kick %players%" - "kick" is added to first level, "%players%" to second level AFTER kick(in addidtion to previous "jerry")
 * 4. "forceteam %players% { blue, red }" - "blue" and "red" added to third level AFTER any from players.. "jerry" is NOT included!!!
 * 
 * but current implementation can't handle such :
 *      "forceteam %players% { blue, red, %maps% } %players%"
 * 
 * %players% would expanded only after "... blue" or "... red" but NOT after "... ut4_map" 
 * 
  **/

///Type thar represents Item in completition model
struct Item {
    ///item which has static childs and dynamic expanders
    QStandardItem*  item;
    ///list of static childs
    Strings         st_list;
    ///list of expander names
    Strings         ex_list;
    ///If this item is dynamically expanded this field holds completition config
    ///config is used for lazy creation of dynamic items
    std::list<QString>         config_list;
};

typedef std::list<Item> Items;

struct rcon::Pimpl{
    Pimpl( const server_id& id, const server_bookmark& options )
        : conn( id, options.rcon_password() )
    {}
    void init() {

#if defined(Q_OS_UNIX)
        QFont f("terminus");
#elif defined(Q_OS_WIN)
        QFont f("");
#endif
        f.setStyleHint(QFont::TypeWriter);
        ui.output->setFont( f );
        ui.input->setFont( f );

        status = new QLabel(0);
        status->setFixedSize( QSize(16, 16) );

        //moving pixmap to Right-Up corner
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget(status);
        vl->addStretch();

        QHBoxLayout* hl = new QHBoxLayout(ui.output);
        hl->addStretch();
        hl->addLayout( vl );

        ui.output->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    }

    void update_colors(){
        rcon_settings s;
        if( !s.custom_colors() )
        {
            QPalette p = ui.output->palette();
            boost::assign::insert(colors)
                (rcon_settings::Background, p.color( QPalette::Base ) )
                (rcon_settings::Text,       p.color( QPalette::Text ) )
                (rcon_settings::Command,    choose_for_background( Qt::yellow, p.color( QPalette::Base ) ) )
                (rcon_settings::Info,       choose_for_background( Qt::cyan,   p.color( QPalette::Base ) ) )
                (rcon_settings::Error,      choose_for_background( Qt::red,    p.color( QPalette::Base ) ) );
        }
        else
        {
            boost::assign::insert(colors)
                (rcon_settings::Background, s.color(rcon_settings::Background) )
                (rcon_settings::Text,       s.color(rcon_settings::Text)       )
                (rcon_settings::Command,    s.color(rcon_settings::Command)    )
                (rcon_settings::Info,       s.color(rcon_settings::Info)       )
                (rcon_settings::Error,      s.color(rcon_settings::Error)      );
        }
    }
    
    Ui_rcon ui;
    QLabel* status;

    std::map<rcon_settings::Color, QColor> colors;

    QStandardItemModel  model;

    Items               items;
    //list of expanders, used by Item through name of expander in Item::ex_list field
    ExpandersByName     expanders;
    
    rcon_connection     conn;
};

///Get Items::iterator linked with \p item, creates new if there is no such Item
Items::iterator get_item( Items& items, QStandardItem* item ){
    Items::iterator it = std::find_if( items.begin(), items.end(), bind(&Item::item, _1) == item );
    if( it == items.end() )
    {
        it = items.insert( items.end(), Item() );
        it->item = item;
    }

    return it;
}

struct find_by_expander: std::binary_function<const Item&, const std::string&, bool>{
    bool operator()( const Item& item, const QString& expander ){
        return std::find( item.ex_list.begin(), item.ex_list.end(), expander ) != item.ex_list.end();
    }
};

///Split string by tokens according to "simple expanding syntax"
QStringList split( const QString& str ){
    static const QRegExp rx( config_rx_c );
    QStringList ret;
    int pos = 0;
    while ( ( pos = rx.indexIn(str, pos) ) != -1 ) {
        pos += rx.cap(0).size();
        ret << rx.cap(0);
    }
    return ret;
}

/**
 * @brief Function for recursevly creating items from config
 * 
 *
 *
 * @param parent - item for wich dynamic and static items are created
 * @param begin - begin iterator of config list ( tokenized "simple syntax for expanders" )
 * @param end - end iterator of config list
 * @param items main storage of all items
 * @param static_item "statifiyer" to handle config tokens as simple words in some case Defaults to true.
 * @return void
 **/
template <typename Iterator>
void create_items( QStandardItem* parent, Iterator begin, Iterator end, Items& items, bool static_item = true){
    static const QRegExp expander_rx(exp_rx_c);
    static const QRegExp group_rx(group_rx_c);

    if( begin == end ) return;
    
    Items::iterator it = get_item( items, parent );
    
    Iterator next(begin); ++next;

    //If element is expander
    if(  expander_rx.exactMatch( *begin ) ){
        it->ex_list.insert( expander_rx.cap(1) );
        //storing config for dynamicaly created items
        it->config_list = std::list<QString>( next, end );
    }
    //If element is grounp
    else if( group_rx.exactMatch( *begin ) ){
        //For ech element in group create items
        BOOST_FOREACH( const QString& sub_str, split( group_rx.cap(1) ) ){
            std::list<typename Iterator::value_type> lst(next, end);
            lst.push_front(sub_str);
            create_items( parent, lst.begin(), lst.end(), items );
        }
    }
    //If element is simple word create item
    else{
        if( static_item ) it->st_list.insert( *begin );
        QStandardItem* item = new QStandardItem( *begin );
        parent->appendRow( item );
        it = get_item( items, item );
        
        create_items( item, next, end, items );
    }
}

rcon::rcon(QWidget* parent, const server_id& id, const server_bookmark& options)
    : QWidget(parent)
    , p_( new Pimpl(id, options) )
{
    p_->ui.setupUi(this);
    p_->init();

    //TODO move to config file
    QStringList lst;
    lst << "%commands%"
        << "kick %players%"
        << "map %maps%"
        << "forceteam %players% { blue, red }" ;
 

    BOOST_FOREACH ( const QString& cmd_str, lst ) {
        QStringList sp = split( cmd_str );
        create_items( p_->model.invisibleRootItem(), sp.begin(), sp.end(), p_->items );
    }
    
    rcon_completer* rc = new rcon_completer(p_->ui.input, &p_->model, this );
    rc->setSeparator(" ");

    connect( p_->ui.input, SIGNAL( returnPressed() ), SLOT( input_enter_pressed() ));

    update_settings();
    
    connect( &p_->conn, SIGNAL( received(const QList<QByteArray>&) ),
             this,      SLOT(received(const QList<QByteArray>&)) );
    
    connect( &p_->conn, SIGNAL(connection_changed(bool)),
             this,      SLOT(set_state(bool)) );
    
    connect( &p_->conn, SIGNAL(players_changed(const QStringList&)),
             this,      SLOT(refresh_players(const QStringList&)) );
    
    connect( &p_->conn, SIGNAL(commands_changed(const QStringList&)),
             this,      SLOT(refresh_commands(const QStringList&)) );
    
    connect( &p_->conn, SIGNAL(maps_changed(const QStringList&)),
             this,      SLOT(refresh_maps(const QStringList&)) );
    
    p_->conn.set_auto_update(true);
}

rcon::~rcon()
{}

void rcon::send_command( const QString& command )
{ p_->conn.send_command( command ); }

void rcon::received( const QList< QByteArray >& data )
{
    Q3ColorMap map;
    map[Q3DefaultColor] = p_->colors[rcon_settings::Text];
    
    BOOST_FOREACH( const QByteArray& line, data ){
        print( Simple, q3coloring(line, map) );
    }
}

void rcon::update_settings()
{
    p_->update_colors();
    QPalette p = p_->ui.output->palette();
    p.setColor( QPalette::Base, p_->colors[rcon_settings::Background] );
    p_->ui.output->setPalette( p );
    p_->ui.output->setAutoFillBackground( rcon_settings().custom_colors() );
}

void rcon::input_enter_pressed()
{
    print( Command, p_->ui.input->text() );
    send_command( p_->ui.input->text() );
    p_->ui.input->clear();
}

void rcon::set_state( bool connected)
{
    if ( !connected )
    {
        p_->status->setPixmap( QPixmap("icons:status-offline.png") );
        print( Info, tr("connection failed") );
    }
    else if( connected )
    {
        p_->status->setPixmap( QPixmap("icons:status-online.png") );
        print( Info, tr("connected") );
    }
}

void rcon::print( TextType type, const QString & text )
{
    QString str = QString("<pre><font face=\"Terminus\",\"monospace\">%1</font></pre>").arg(colorize_string(type, text));
    if( !text.isEmpty() )
        p_->ui.output->append( str );
}

QString rcon::colorize_string( rcon::TextType type, const QString& text ) const
{
    switch (type)
    {
        case rcon::Command: return colorize( text, p_->colors[rcon_settings::Command] );
        case rcon::Simple:  return colorize( text, p_->colors[rcon_settings::Text] );
        case rcon::Info:    return colorize( text, p_->colors[rcon_settings::Info] );
        case rcon::Error:   return colorize( text, p_->colors[rcon_settings::Error] );
        default:
            return text;
    }    
}

void rcon::refresh_expander( const QString& exp )
{
    std::for_each(
        make_filter_iterator( bind( find_by_expander(), _1, exp ), p_->items.begin(), p_->items.end() ),
        make_filter_iterator( bind( find_by_expander(), _1, exp ), p_->items.end()  , p_->items.end() ),
        bind( &rcon::update_item, this, _1)
    );
}

void rcon::refresh_players(const QStringList& players)
{
    p_->expanders["players"] = Strings( players.begin(), players.end() );
    refresh_expander("players");
}

void rcon::refresh_commands(const QStringList& commands)
{
    p_->expanders["commands"] = Strings( commands.begin(), commands.end() );
    refresh_expander("commands");
}

void rcon::refresh_maps(const QStringList& maps)
{
    p_->expanders["maps"] = Strings( maps.begin(), maps.end() );
    refresh_expander("maps");
}

///Erase item \p item and all of it's childs from our \p items
void erase_item( QStandardItem* item, Items& items ){
    for( int i = 0; i < item->rowCount(); ++i )
        erase_item(item->child( i ), items);

    Items::iterator it = std::find_if( items.begin(), items.end(), bind(&Item::item, _1) == item );
    if( it != items.end() )
        items.erase( it );
}

void rcon::update_item(const Item& item)
{
    LOG_HARD << "Updating item " << item.item->text().toStdString();
    QList<QStandardItem*> to_delete;

    //List of all autocompletition elements of item
    Strings full_list;
    //...static items
    std::copy( item.st_list.begin(), item.st_list.end(), std::inserter( full_list, full_list.end() ) );

    //...dynamically expanded items
    BOOST_FOREACH( const QString& exp, item.ex_list )
        std::copy( p_->expanders[exp].begin(), p_->expanders[exp].end(), std::inserter( full_list, full_list.end() ) );

    //checking QStandardItemModel structure
    QStandardItem* child(0);
    Strings::iterator cmd;
    for( int i = 0; i < item.item->rowCount(); ++i ) {
        child = item.item->child( i );

        //Item exixst all ok
        if( cmd = full_list.find( child->text() ), cmd != full_list.end() )
            full_list.erase( cmd );
        // Item does not exist and model item must be removed
        else
            to_delete.push_back( child );
    }

    BOOST_FOREACH( QStandardItem* it, to_delete   )
    {
        LOG_HARD << "Removing item" << it->text().toStdString();
        erase_item( it, p_->items );
        p_->model.removeRow( it->row(), item.item->index() );        
    }

    BOOST_FOREACH( const QString& cmd, full_list )
    {
        if ( item.config_list.empty() )
            item.item->appendRow( new QStandardItem( cmd ) );
        else
        {
            std::list<QString> lst( item.config_list.begin(), item.config_list.end() );
            lst.push_front(cmd);
            LOG_HARD << "Creating dynamyc item" << cmd.toStdString();
            create_items( item.item, lst.begin(), lst.end(), p_->items, false);
        }
        
    }
    LOG_HARD << "Items size:" << p_->items.size();
}




