#include <algorithm>
#include <vector>
//#include <stdexcept>

#include <common/exception.h>
#include <boost/bind.hpp>
#include <boost/assign/std/map.hpp>
#include <boost/assign/list_of.hpp>

#include <QAction>
#include <QString>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QDockWidget>
#include <QStyle>
#include <QTextDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QComboBox>
#include <QTextStream>
#include <QApplication>

#include "tools.h"
#include "notificator.h"
#include "qt_syslog.h"

SYSLOG_MODULE(tools)

using namespace std;

QAction* add_separator_action(QWidget* w, const QString& text)
{
    QAction* separator = new QAction(w);
    separator->setSeparator(true);
    separator->setText(text);
    w->addAction(separator);
    
    return separator;
}

const Q3ColorMap& default_colors()
{
    static Q3ColorMap colors;

    if( colors.empty() )
    {
        boost::assign::insert( colors )
            ( Q3Black,   Qt::black   )
            ( Q3Red,     Qt::red     )
            ( Q3Green,   Qt::green   )
            ( Q3Yellow,  Qt::yellow  )
            ( Q3Blue,    Qt::blue    )
            ( Q3Cyan,    Qt::cyan    )
            ( Q3Magenta, Qt::magenta )
            ( Q3White,   Qt::white   );
    }

    return colors;
}


const QColor& color( Q3Color c, const Q3ColorMap& substitute )
{
    if( substitute.find(c) != substitute.end() )
        return substitute.find(c)->second;
    else
        return default_colors().find(c)->second;
}


const QColor& color( const QString & str, const Q3ColorMap& substitute )
{
    bool ok;
    static const int colors_count = default_colors().size();

    Q3Color c = Q3Color( str.toInt(&ok) % colors_count );
    if (!ok) c = Q3DefaultColor;

    return color( c, substitute );
}

QString colorize( const QString& token, const QColor& color )
{
    static QString colored("<font color=\"%1\">%2</font>");
    return colored.arg( color.name(), /*Qt::escape*/(token) );
}

QString colorize_token(const QString& str, const Q3ColorMap& substitute )
{
    static const QRegExp color_rx("^\\^(\\d).*");
    if( color_rx.exactMatch(str) )
        return colorize( str.right( str.size() -2 ), color( color_rx.cap(1), substitute ) );
    else
        return colorize( str, color(Q3DefaultColor, substitute) );
}

QStringList split_by_q3colors( const QString& str){
    QStringList ret;
    
    int index_begin = 0;
    int index_end = 0;
    
    while( index_end = str.indexOf( QRegExp("\\^(\\d)"), index_end ), index_end != -1 ){
        ret << str.mid( index_begin, index_end - index_begin );
        
        index_begin = index_end;
        index_end++;
    }
    
    ret << str.mid( index_begin, -1 );
    
    return ret;
}

QString q3coloring( const QString & str, const Q3ColorMap& custom )
{
    QStringList lst = split_by_q3colors(str);

    //replacing quake3 color-markers by html-formated text
    std::transform(lst.begin(), lst.end(), lst.begin(), boost::bind(colorize_token, _1, boost::ref(custom)) );
    
    //joining list to plain string
    return lst.join("");
}

QString q3stripcolor(const QString& str)
{
    return QString(str).replace( QRegExp("\\^\\d"), QString() );
}

QString toplainhtml(const QString& str)
{
    return Qt::escape( q3stripcolor(str) );
}

QColor choose_for_background(Qt::GlobalColor standard, const QColor& background)
{
    static std::map<Qt::GlobalColor, QColor> light_colors;
    static std::map<Qt::GlobalColor, QColor> dark_colors;

    if( light_colors.empty() )
    {
        boost::assign::insert( light_colors )
            ( Qt::white,  Qt::white )
            ( Qt::red,    Qt::red   )
            ( Qt::green,  Qt::green )
            ( Qt::blue,   Qt::blue  )
            ( Qt::cyan,   Qt::cyan  )
            ( Qt::magenta,Qt::magenta )
            ( Qt::yellow, Qt::yellow  )
            ( Qt::gray,   Qt::gray  );
    }

    if( dark_colors.empty() )
    {
        boost::assign::insert( dark_colors )
            ( Qt::white,  Qt::black )
            ( Qt::red,    Qt::darkRed   )
            ( Qt::green,  Qt::darkGreen )
            ( Qt::blue,   Qt::darkBlue  )
            ( Qt::cyan,   Qt::darkCyan  )
            ( Qt::magenta,Qt::darkMagenta )
            ( Qt::yellow, Qt::darkYellow  )
            ( Qt::gray,   Qt::darkGray  );
    }

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    if( background.lightness() > 128 )
#else
    if (true)
#endif
        return dark_colors[standard];
    else
        return light_colors[standard];
}

QString common_substring_from_begin(const QString& s1, const QString& s2)
{
    QString res;
    int sz = qMin(s1.length(), s2.length());
    for (int i = 0; i < sz; i++)
    {
        const QChar& ch1 = s1[i];
        const QChar& ch2 = s2[i];
        if (ch1 == ch2)
            res += ch1;
        else
            break;
    }
    return res;
}

QString common_substring_from_end(const QString& s1, const QString& s2)
{
    QString res;
    int i1 = s1.length() - 1;
    int i2 = s2.length() - 1;
    while (i1 > 0 && i2 > 0)
    {
        const QChar& ch1 = s1[i1];
        const QChar& ch2 = s2[i2];
        if (ch1 == ch2)
            res = ch1 + res;
        else
            break;
        i1--;
        i2--;
    }
    return res;
}


std::set< Gear > forbidden(GearMask mask)
{
    std::set<Gear> ret;
    
    for( Gear g = Grenades; g <= Negev; g = Gear( g*2 ) ){
        if ( !is_allowed(g, mask) ) ret.insert(g);
    }

    return ret;
}

std::set< Gear > allowed(GearMask mask)
{
    std::set<Gear> ret;
    
    for( Gear g = Grenades; g <= Negev; g = Gear( g*2 ) ){
        if ( is_allowed(g, mask) ) ret.insert(g);
    }

    return ret;
}

GearMask to_mask(const std::set< Gear >& st)
{
    GearMask m(0);
    std::set<Gear>::const_iterator it = st.begin();
    
    for( ; it != st.end(); ++it ){
        m |= *it;
    }
    
    return m;
}


QString gear(Gear g)
{
    static const std::map<Gear, QString> gear = boost::assign::map_list_of
        ( Grenades, QObject::tr("Grenades")  )
        ( Snipers,  QObject::tr("Snipers")   )
        ( Spas,     QObject::tr("Spas")      )
        ( Pistols,  QObject::tr("Pistols")   )
        ( Automatic,QObject::tr("Automatic") )
        ( Negev,    QObject::tr("Negev")     );
    
    return ( gear.find(g) != gear.end() )
        ? gear.find(g)->second
        : "Unknown";
}


QString icon(Gear g)
{
    static const std::map<Gear, QString> icons = boost::assign::map_list_of
        ( Grenades, "icons:weapons/grenade.png" )
        ( Snipers,  "icons:weapons/snipe.png"   )
        ( Spas,     "icons:weapons/spas.png"    )
        ( Pistols,  "icons:weapons/pistol.png"  )
        ( Automatic,"icons:weapons/autos.png"   )
        ( Negev,    "icons:weapons/negev.png"   );
        
    return ( icons.find(g) != icons.end() )
        ? icons.find(g)->second
        : "Unknown";
}

void load_app_style_sheet(const QString& file_name)
{
    if (file_name.isEmpty())
    {
        qApp->setStyleSheet(QString());
        return;
    }
    QFile f(file_name);
    if (!f.exists())
        throw qexception(QObject::tr("File \"%1\" does not exists").arg(f.fileName()));

    if (!f.open(QFile::ReadOnly))
        throw qexception(QObject::tr("Cannot open file \"%1\" to read").arg(f.fileName()));

    QDir::setSearchPaths ( "skin", QStringList( QFileInfo( file_name ).absoluteDir().absolutePath() ) );
    
    QTextStream ts(&f);
    QString s = ts.readAll();

    qApp->setStyleSheet(s);
}

int combo_find_item(QComboBox* c, const QVariant& v)
{
    for (int i = 0; i < c->count(); i++)
    {
        if (c->itemData(i, Qt::UserRole) == v)
            return i;
    }
    return -1;
}

void show_error(const QString& msg)
{
    QIcon ico = qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical);
    Notificator::showMessage(ico, QString(), msg);
    LOG_ERR << msg;
}

