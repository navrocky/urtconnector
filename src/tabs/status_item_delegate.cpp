#include <QPainter>

#include <common/server_info.h>
#include <common/server_list.h>
#include "common_item_tags.h"
#include "status_item_delegate.h"

////////////////////////////////////////////////////////////////////////////////
// status_item_delegate

status_item_delegate::status_item_delegate(server_list_p sl, QObject* parent)
: QStyledItemDelegate(parent)
, sl_(sl)
{
}

void status_item_delegate::paint(QPainter* painter, 
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    //Draw base styled-item(gradient backgroud and other)
    QStyledItemDelegate::paint(painter, option, index);

    if (index.data(c_suppress_role).toBool())
        return;

    //Rect for drawing icons
    const QRect& optr = option.rect;
    QRect icon_rect(optr.x() + 2, optr.y() + 2, optr.height() - 4, optr.height() - 4);

    server_info_p si = (sl_)
            ? sl_->get(index.data(c_id_role).value<server_id > ()) : server_info_p();

    if (!si)
        si = server_info_p(new server_info());

    static QPixmap icon_none("icons:status-none.png");
    static QPixmap icon_online("icons:status-online.png");
    static QPixmap icon_offline("icons:status-offline.png");
    static QPixmap icon_updating("icons:status-update.png");
    static QPixmap icon_passwd("icons:status-passwd.png");
    static QPixmap icon_empty;

    QPixmap& icon_status = icon_empty;

    if (si->updating)
        icon_status = icon_updating;
    else
        switch (si->status)
        {
        case server_info::s_none:
            icon_status = icon_none;
            break;
        case server_info::s_up:
            icon_status = icon_online;
            break;
        case server_info::s_down:
            icon_status = icon_offline;
            break;
        }

    //First icon - status icon
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(icon_rect, icon_status);

    next_icon(icon_rect);

    if (si->is_password_needed()) painter->drawPixmap(icon_rect, icon_passwd);

    next_icon(icon_rect);

    if (si->get_info("pure", "-1").toInt() == 0)
        painter->drawPixmap(icon_rect, QPixmap("icons:user-identity.png"));
    
    next_icon(icon_rect);

    if ( !si->forbidden_gears().empty() )
        painter->drawPixmap(icon_rect, QPixmap("icons:weapons/pistol.svg"));
}

void status_item_delegate::next_icon(QRect& icon) const
{
    icon.adjust(icon.width(), 0, icon.width(), 0);
}

QSize status_item_delegate::sizeHint(const QStyleOptionViewItem & option,
                                     const QModelIndex & index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    if (sz.height() < 16)
        sz.setHeight(16);
    return sz;
}
