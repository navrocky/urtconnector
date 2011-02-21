#ifndef STATUS_ITEM_DELEGATE_H
#define	STATUS_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

#include "../pointers.h"

class QStyleOptionViewItem;

class status_item_delegate : public QStyledItemDelegate
{
public:
    status_item_delegate(server_list_p sl, QObject* parent);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;

//    void set_server_list(const server_list_p& sl)
//    {
//            sl_ = sl;
//    }

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
    void next_icon(QRect& icon) const;

    server_list_p sl_;
};

#endif	/* STATUS_ITEM_DELEGATE_H */

