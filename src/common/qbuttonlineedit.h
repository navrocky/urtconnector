/* 
 * File:   qbuttonlineedit.h
 * Author: navrocky.vlad
 */

#ifndef QBUTTONLINEEDIT_H
#define	QBUTTONLINEEDIT_H

#include <QLineEdit>
#include <QToolButton>
#include <QList>
#include <QMap>

class QButtonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    enum Position
    {
        Left,
        Right
    };

    QButtonLineEdit(QWidget* parent = NULL);

    void addWidget(QWidget* w, Position pos = Right);
    void insertWidget(int index_before, QWidget* w, Position pos = Right);
    void removeWidget(QWidget* w);
    QWidget* takeWidget(QWidget* w);

    QToolButton* addButton(Position pos = Right);
    QToolButton* addActionButton(QAction* a, Position pos = Right);

    virtual bool event(QEvent* e);

protected:
    virtual QSize minimumSizeHint() const;
    virtual void resizeEvent(QResizeEvent* e);
    
private slots:
    void widgetDeleted(QObject*);

private:
    int calcWidgetsWidth(Position pos) const;
    int calcMaxHeight(Position pos) const;
    QSize widgetSize(QWidget* w) const;
    void arrangeWidgets();
    QWidget* takeWidgetInt(QWidget* w);

    typedef QList<QWidget*> Widgets;
    typedef QMap<Position, Widgets> AllWidgets;

    AllWidgets m_widgets;
    int m_space;
};

#endif	/* QBUTTONLINEEDIT_H */
