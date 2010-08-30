
#include <QStyleOption>
#include <QPainter>


#include "iconned_dock_style.h"


namespace {
    QPoint oxygen_icon_position( const QRect& title_rect, const QSize& icon_size )
    {
        return QPoint( 5 + title_rect.left(), title_rect.center().y() - icon_size.width()/2 + 1 );
    }

    QPoint plastiq_icon_position( const QRect& title_rect, const QSize& icon_size )
    {
        return QPoint( 1 + title_rect.left(), title_rect.center().y() - icon_size.width()/2 + 1 );
    }

    QPoint default_icon_position( const QRect& title_rect, const QSize& icon_size )
    {
        return QPoint( 2 + title_rect.left(), title_rect.center().y() - icon_size.width()/2 + 1 );
    }
}



iconned_dock_style::iconned_dock_style(const QIcon& icon, QStyle* style)
    : QProxyStyle(style)
    , icon_(icon)
{}

QString iconned_dock_style::make_spaces(int width, const QFontMetrics& metrics) const
{
    QString spaces;
    while( metrics.width(spaces) < width )
        spaces.append(" ");

    return spaces;
}

void iconned_dock_style::drawControl(QStyle::ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
 {
    if( element == QStyle::CE_DockWidgetTitle)
    {
        //width of the icon
        int width = std::min( pixelMetric(QStyle::PM_SmallIconSize), option->rect.height() );

        QSize icon_size( width, width );
        
        QString spaces = make_spaces( width, option->fontMetrics );
        int spaces_width = option->fontMetrics.width(spaces);

        
        const QStyleOptionDockWidget* s = static_cast<const QStyleOptionDockWidget*>(option);

        QPoint icon_point;

        //calculating top-left icon point( hard-hack around terrible oxygen-style )
        if( baseStyle()->objectName() == "oxygen" )
        {
            const_cast<QStyleOptionDockWidget*>(s)->title.prepend( spaces );
            icon_point = oxygen_icon_position( option->rect, icon_size );
        }
        else if( baseStyle()->objectName().startsWith("plast") )
        {
            icon_point = plastiq_icon_position( option->rect, icon_size );
        }
        else
        {
            const_cast<QStyleOptionDockWidget*>(s)->title.prepend( spaces );
            icon_point = default_icon_position( option->rect, icon_size );
        }

        baseStyle()->drawControl(element, option, painter, widget);

        //drawing pixmap
        painter->drawPixmap(icon_point, icon_.pixmap( width, width ) );
    }
    else
        baseStyle()->drawControl(element, option, painter, widget);
}


