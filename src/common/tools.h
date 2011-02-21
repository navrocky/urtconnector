#ifndef TOOLS_H
#define TOOLS_H

#include <map>

#include <boost/function.hpp>

#include <QObject>
#include <QPainter>

class QColor;
class QWidget;
class QString;
class QAction;
class QDockWidget;
class QLabel;
class QIcon;


enum Q3Color {
    Q3Black,
    Q3Red,
    Q3Green,
    Q3Yellow,
    Q3Blue,
    Q3Cyan,
    Q3Magenta,
    Q3White
};

static const Q3Color Q3DefaultColor = Q3White;

typedef std::map<Q3Color, QColor> Q3ColorMap;

///Map of Quake 3 defaults colors
const Q3ColorMap& default_colors();

///Get color from color map by id, uses substitute map if exists
const QColor& color( Q3Color c, const Q3ColorMap& substitute = Q3ColorMap() );

///Get color from color map by string-id, uses substitute map if exists
const QColor& color( const QString& str, const Q3ColorMap& custom = Q3ColorMap() );

///Colorize string like html: <font color="color.name">text</font>
QString colorize( const QString& token, const QColor& color );

///Find Quake 3 color mark in string and add apropriate html color tags
QString q3coloring( const QString& str, const Q3ColorMap& custom = Q3ColorMap()  );

/*! Adds menu separator action to widget with optional text. */
QAction* add_separator_action(QWidget* w, const QString& text = "");

///test function
QColor choose_for_background( Qt::GlobalColor standard, const QColor& background );
QString common_substring_from_begin(const QString& s1, const QString& s2);
QString common_substring_from_end(const QString& s1, const QString& s2);


class qt_signal_wrapper: public QObject
{
    Q_OBJECT
public:
    typedef boost::function < void () > function;
    qt_signal_wrapper( QObject* parent, function function = 0 )
            : QObject( parent )
    {
        if ( function != 0 )
            functions_.push_back( function );
    };

    ~qt_signal_wrapper(){};

    void add_function( function function )
    { functions_.push_back( function ); }

public slots:
    void activate()
    {
        try {
        for ( uint i = 0; i < functions_.size();++i )
            functions_[i]();
        }
        catch(...){}
    }

private:
    std::vector<function> functions_;
};



#endif