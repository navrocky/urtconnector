#ifndef TOOLS_H
#define TOOLS_H

class QWidget;
class QString;
class QAction;

/*! Adds menu separator action to widget with optional text. */
QAction*  add_separator_action(QWidget* w, const QString& text = "");

#endif
