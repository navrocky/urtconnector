#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>

class QColor;
class QWidget;
class QString;
class QAction;
class QDockWidget;
class QLabel;
class QIcon;

/*! Adds menu separator action to widget with optional text. */
QAction*  add_separator_action(QWidget* w, const QString& text = "");

QColor color( const QString& str );

QString colorize( const QString& str, const QColor& color );

QString make_css_colored(QString str, const QString& skip = QString());

QString q3coloring( const QString& str, const QString& skip = QString() );

///test function
QColor choose_for_background( Qt::GlobalColor standard, const QColor& background );

#include <iostream>
class iconned_dock_style: public QProxyStyle{
    Q_OBJECT
    QIcon icon_;
public:
    iconned_dock_style(const QIcon& icon,  QStyle* style = 0 )
        : QProxyStyle(style)
        , icon_(icon)
    {}
    virtual ~iconned_dock_style(){};

    std::pair<QString,int> make_spaces( int width, const QFontMetrics& metrics ) const
    {
        QString spaces;
        while( metrics.width(spaces) < width )
            spaces.append(" ");

        return std::make_pair( spaces, metrics.width(spaces) );
    }

    virtual void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const
    { 
        if( element == QStyle::CE_DockWidgetTitle)
        {
            //width of the icon
            int width = std::min( pixelMetric(QStyle::PM_SmallIconSize), option->rect.height() );

            const QStyleOptionDockWidget* s = static_cast<const QStyleOptionDockWidget*>(option);

            std::pair<QString, int> spaces = make_spaces( width, option->fontMetrics );

            int w2 = spaces.second / 2 - width / 2;

            std::cerr<<"Width:"<<width<<" spaces:"<<spaces.second<<" w2:"<<w2<<std::endl;

            int mr = baseStyle()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);
            std::cerr<<"St margin:"<< mr << std::endl;

            std::cerr<<"Descend:"<< option->fontMetrics.descent() << std::endl;

            QRect titleRect = baseStyle()->subElementRect(SE_DockWidgetTitleBarText, option, widget);
            
            std::cerr<<"Title rect L:"<< titleRect.left() << std::endl;

            
//             QString stored_title = s->title;
//             const_cast<QStyleOptionDockWidget*>(s)->title.clear();

            const_cast<QStyleOptionDockWidget*>(s)->title.prepend( spaces.first );

            QPoint icon_point;
            
            //calculating top-left icon point( hard-hack around terrible oxygen-style )
            if( baseStyle()->objectName().toStdString() == "oxygen" )
            {
                icon_point = QPoint( 3 + w2 + option->rect.left(), option->rect.center().y() - width/2 + 1 );
            }
            else
            {
                icon_point = QPoint( w2 + option->fontMetrics.descent() + option->rect.left(), option->rect.center().y() - width/2 + 1 );
            }

            //drawing default-styled title back
//             baseStyle()->drawControl(element, option, painter, widget);

            //Adjusting title rect with icon width( hard-hack around terrible oxygen-style )
//             if( baseStyle()->objectName().toStdString() == "oxygen" )
//                 const_cast<QStyleOption*>(option)->rect = option->rect.adjusted( width, 0, 0, 0);
//             else
//                 const_cast<QStyleOption*>(option)->rect = option->rect.adjusted( 2 + width, 0, 0, 0);

            //restoring title text
//             const_cast<QStyleOptionDockWidget*>(s)->title = stored_title;

            baseStyle()->drawControl(element, option, painter, widget);

            //drawing pixmap
            painter->drawPixmap(icon_point, icon_.pixmap( width, width ) );
        }
        else

            //drawing default(but adjusted if any) title
            baseStyle()->drawControl(element, option, painter, widget);
    }
};

QString common_substring_from_begin(const QString& s1, const QString& s2);
QString common_substring_from_end(const QString& s1, const QString& s2);


#endif
