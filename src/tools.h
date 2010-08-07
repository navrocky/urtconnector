#ifndef TOOLS_H
#define TOOLS_H

class QColor;
class QWidget;
class QString;
class QAction;

/*! Adds menu separator action to widget with optional text. */
QAction*  add_separator_action(QWidget* w, const QString& text = "");


QColor color( const QString& str );

QString colorize( const QString& str, const QColor& color );

QString make_css_colored(QString str, const QString& skip = QString());

QString q3coloring( const QString& str, const QString& skip = QString() );

#endif
