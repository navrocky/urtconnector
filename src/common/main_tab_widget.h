#ifndef MAIN_TAB_WIDGET_H
#define	MAIN_TAB_WIDGET_H

#include <QTabWidget>

/*!
 * Tab icon and caption synchronization with the widget title and icon
 */
class main_tab_widget : public QTabWidget
{
    Q_OBJECT
public:
    main_tab_widget(QWidget* parent);
    
    void add_widget(QWidget* w);
    
    virtual bool eventFilter(QObject* watched, QEvent* event);
private:
    void sync_tab_info(QWidget* w);
};

#endif	/* MAIN_TAB_WIDGET_H */

