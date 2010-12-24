/* 
 * File:   qbuttonlineedit.cpp
 * Author: vlad
 * 
 * Created on 21 Декабрь 2010 г., 14:30
 */

#include <QVBoxLayout>
#include <QStyle>
#include <QWidget>
#include <QEvent>

#include "qbuttonlineedit.h"

QButtonLineEdit::QButtonLineEdit(QWidget* parent)
: QLineEdit(parent)
, m_space(1)
{
}

void QButtonLineEdit::addWidget(QWidget* w, Position pos)
{
    m_widgets[pos].append(w);
    w->setParent(this);
    connect(w, SIGNAL(destroyed(QObject*)), SLOT(widgetDeleted(QObject*)));
    arrangeWidgets();
    updateGeometry();
}

void QButtonLineEdit::insertWidget(int index_before, QWidget* w, Position pos)
{
    m_widgets[pos].insert(index_before, w);
    connect(w, SIGNAL(destroyed(QObject*)), SLOT(widgetDeleted(QObject*)));
    arrangeWidgets();
    updateGeometry();
}

void QButtonLineEdit::widgetDeleted(QObject* o)
{
    takeWidgetInt(qobject_cast<QWidget*>(o));
}

QWidget* QButtonLineEdit::takeWidgetInt(QWidget* w)
{
    QWidget* res = NULL;

    Widgets& lw = m_widgets[Left];
    int i = lw.indexOf(w);
    if (i >= 0)
    {
        res = lw[i];
        lw.removeAt(i);
    }

    if (!res)
    {
        Widgets& rw = m_widgets[Right];
        i = rw.indexOf(w);
        if (i >= 0)
        {
            res = rw[i];
            rw.removeAt(i);
        }
    }

    if (res)
    {
        arrangeWidgets();
        updateGeometry();
    }

    return res;
}

QWidget* QButtonLineEdit::takeWidget(QWidget* w)
{
    disconnect(w, SLOT(widgetDeleted(QObject*)));
    return takeWidgetInt(w);
}

void QButtonLineEdit::removeWidget(QWidget* w)
{
    delete w;
}

QSize QButtonLineEdit::minimumSizeHint() const
{
    QSize sz = QLineEdit::minimumSizeHint();

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    QSize lsz = QSize(calcWidgetsWidth(Left), calcMaxHeight(Left));
    QSize rsz = QSize(calcWidgetsWidth(Right), calcMaxHeight(Right));

    QSize res(sz.width() + lsz.width() + rsz.width(),
              qMax(qMax(lsz.height(), rsz.height()) + frameWidth * 2, sz.height()));
    return res;
}

int QButtonLineEdit::calcWidgetsWidth(Position pos) const
{
    int res = 0;
    const Widgets& widgets = m_widgets[pos];
    if (widgets.size() > 0)
    {
        foreach (QWidget* w, widgets)
        {
            if (w->isVisibleTo(const_cast<QButtonLineEdit*>(this)))
                res += widgetSize(w).width() + m_space;
        }
        res -= m_space;
    }
    return res;
}

int QButtonLineEdit::calcMaxHeight(Position pos) const
{
    int res = 0;
    foreach (QWidget* w, m_widgets[pos])
    {
        if (w->isVisibleTo(const_cast<QButtonLineEdit*>(this)))
           res = qMax(res, widgetSize(w).height());
    }
    return res;
}

QSize QButtonLineEdit::widgetSize(QWidget* w) const
{
    // TODO more smart widget width detection needed, based on size policy like a QLayout
    QSize res = w->sizeHint();
    if (!res.isValid())
        res = w->minimumSize();
    if (!res.isValid())
        res = w->minimumSizeHint();
    if (!res.isValid())
        res = w->size();
    return res;
}

void QButtonLineEdit::resizeEvent(QResizeEvent*)
{
    arrangeWidgets();
}

bool QButtonLineEdit::event(QEvent* e)
{
    if (e->type() == QEvent::LayoutRequest)
        arrangeWidgets();
    return QLineEdit::event(e);
}

void QButtonLineEdit::arrangeWidgets()
{
    int lw = calcWidgetsWidth(Left);
    int rw = calcWidgetsWidth(Right);
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    setStyleSheet(QString("QLineEdit { padding-left: %1px; padding-right: %2px; } ")
        .arg(frameWidth + lw).arg(frameWidth + rw));

    const Widgets& l_widgets = m_widgets[Left];
    int h = rect().height() - frameWidth * 2;
    int left = frameWidth;
    for (int i = 0; i < l_widgets.size(); i++)
    {
        QWidget* w = l_widgets[i];
        if (!w->isVisibleTo(this))
            continue;
        QSize sz = widgetSize(w);
        QRect r(left, (h - sz.height()) / 2 + frameWidth, sz.width(), sz.height());
        w->setGeometry(r);
        left += sz.width();
    }

    const Widgets& r_widgets = m_widgets[Right];
    int right = rect().right() - frameWidth;
    for (int i = r_widgets.size() - 1; i >= 0; i--)
    {
        QWidget* w = r_widgets[i];
        QSize sz = widgetSize(w);
        right -= sz.width();
        QRect r(right, (h - sz.height()) / 2 + frameWidth, sz.width(), sz.height());
        w->setGeometry(r);
    }
}

QToolButton* QButtonLineEdit::addButton(Position pos)
{
    QToolButton* btn = new QToolButton(this);
    btn->setCursor(Qt::ArrowCursor);
    btn->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    addWidget(btn, pos);
    return btn;
}

QToolButton* QButtonLineEdit::addActionButton(QAction* a, Position pos)
{
    QToolButton* btn = addButton(pos);
    btn->setDefaultAction(a);
    return btn;
}

