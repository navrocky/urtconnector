#include <QEvent>

#include "main_tab_widget.h"

main_tab_widget::main_tab_widget(QWidget* parent)
: QTabWidget(parent)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    setDocumentMode(true);
#endif
}

void main_tab_widget::add_widget(QWidget* w)
{
    addTab(w, QString());
    sync_tab_info(w);
    w->installEventFilter(this);
}

bool main_tab_widget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::WindowTitleChange ||
        event->type() == QEvent::WindowIconChange ||
        event->type() == QEvent::ToolTipChange)
    {
        QWidget* w = qobject_cast<QWidget*>(watched);
        int i = indexOf(w);
        if (event->type() == QEvent::WindowTitleChange)
            setTabText(i, w->windowTitle());
        else if (event->type() == QEvent::WindowIconChange)
            setTabIcon(i, w->windowIcon());
        else if (event->type() == QEvent::ToolTipChange)
            setTabToolTip(i, w->toolTip());
    }

    return QTabWidget::eventFilter(watched, event);
}

void main_tab_widget::sync_tab_info(QWidget* w)
{
    int i = indexOf(w);
    setTabText(i, w->windowTitle());
    setTabIcon(i, w->windowIcon());
    setTabToolTip(i, w->toolTip());
}
