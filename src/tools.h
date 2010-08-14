#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>

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



///Class that hacks QDockWidget title to show an icon. Legal icon realiation will be added in later Qt releases
class title_icon_adder: public QObject{
    QDockWidget* dock;
    QLabel* title;
    QLabel* icon;
public:
    explicit title_icon_adder(QDockWidget* d, const QString& t, const QIcon& p);
    virtual ~title_icon_adder();

protected:
    virtual bool eventFilter(QObject* o, QEvent* e);
};


#endif
