#include <algorithm>
#include <vector>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/assign/std/map.hpp>

#include <QAction>
#include <QString>
#include <QWidget>
#include <QLabel>
#include <QEvent>
#include <QDockWidget>
#include <QStyle>

#include "tools.h"

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
    return colored.arg( color.name(), token);
}

QString colorize_token(const QString& str, const Q3ColorMap& substitute )
{
    static const QRegExp color_rx("^(\\d).*");
    if( color_rx.exactMatch(str) )
        return colorize( str.right( str.size() -1 ), color( color_rx.cap(1), substitute ) );
    else
        return str;
}

QString q3coloring(const QString & str, const Q3ColorMap& custom)
{
    //split incoming string by quake3 color-markers
    QStringList lst = str.split( "^" );
    //removing empty lines
    lst.erase( std::remove_if( lst.begin(), lst.end(), bind( &QString::isEmpty, _1 ) ), lst.end() );
    //replacing quake3 color-markers by html-formated text
    std::transform(lst.begin(), lst.end(), lst.begin(), boost::bind(colorize_token, _1, boost::ref(custom)) );
    //joining list to plain string
    return lst.join("");
}

QString q3stripcolor(const QString& str)
{
    return QString(str).replace( QRegExp("\\^\\d"), QString() );
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
