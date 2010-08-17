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

class iconned_dock_style: public QProxyStyle{
    Q_OBJECT
    QIcon icon_;
public:
    iconned_dock_style(const QIcon& icon,  QStyle* style = 0 )
        : QProxyStyle(style)
        , icon_(icon)
    {}
    virtual ~iconned_dock_style(){};

    virtual void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const
    {
        if( element == QStyle::CE_DockWidgetTitle)
        {
            //width of the icon
            int width = pixelMetric(QStyle::PM_ToolBarIconSize);
            //margin of title from frame
            int margin = baseStyle()->pixelMetric( QStyle::PM_DockWidgetTitleMargin );
            //spacing between icon and title
            int spacing = baseStyle()->pixelMetric( QStyle::PM_LayoutHorizontalSpacing );
          
            QPoint icon_point( margin + option->rect.left(), margin + option->rect.center().y() - width/2 );
            
            painter->drawPixmap(icon_point, icon_.pixmap( width, width ) );
            
            const_cast<QStyleOption*>(option)->rect = option->rect.adjusted(width, 0, 0, 0);
        }
        baseStyle()->drawControl(element, option, painter, widget);
    }
};

#endif
