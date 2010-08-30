#ifndef ICONNED_DOCK_STYLE_H
#define ICONNED_DOCK_STYLE_H

#include <QProxyStyle>
#include <QIcon>

class iconned_dock_style: public QProxyStyle{
    Q_OBJECT
    QIcon icon_;
public:
    iconned_dock_style(const QIcon& icon,  QStyle* style = 0 );
    
    virtual ~iconned_dock_style(){};

    QString make_spaces( int width, const QFontMetrics& metrics ) const;

    virtual void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;

};

#endif