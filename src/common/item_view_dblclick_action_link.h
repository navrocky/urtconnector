#ifndef _ITEM_VIEW_DBLCLICK_ACTION_LINK_H
#define	_ITEM_VIEW_DBLCLICK_ACTION_LINK_H

#include <QObject>

class QAbstractItemView;
class QAction;
class QModelIndex;

class item_view_dblclick_action_link: public QObject
{
    Q_OBJECT
public:
    item_view_dblclick_action_link(QObject* parent, QAbstractItemView* view,
                                   QAction* action);
private slots:
    void double_clicked(const QModelIndex&);

private:
    QAction* a_;
};

#endif	/* _ITEM_VIEW_DBLCLICK_ACTION_LINK_H */

