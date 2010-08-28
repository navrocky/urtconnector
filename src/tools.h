#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
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
QString common_substring_from_begin(const QString& s1, const QString& s2);
QString common_substring_from_end(const QString& s1, const QString& s2);

#endif
