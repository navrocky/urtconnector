
#include <boost/foreach.hpp>

#include <QObject>
#include <QTreeWidgetItem>

#include <common/server_id.h>
#include <common/server_info.h>
#include <common/server_list.h>
#include "../pointers.h"
#include "common_item_tags.h"
#include "tab_context.h"
#include <geoip/geoip.h>

#include "tools.h"

void update_server_info_item(const tab_context& ctx, QTreeWidgetItem* item)
{
    const server_id& id = item->data(0, c_id_role).value<server_id>();
    if (id.is_empty())
        return;

    static const server_info_p empty( new server_info );

    server_info_p si = ctx.serv_list()->get(id);
    if (!si)
        si = empty;

    int stamp = item->data(0, c_stamp_role).value<int>();
    if ( si->update_stamp() != stamp || stamp == 0 )
    {
        QString name = si->name;
        const server_bookmark& bm = ctx.bookmarks()->get(id);
        if (!bm.is_empty())
        {
            if (!bm.name().isEmpty() && name != bm.name())
            {
                if (name.isEmpty())
                    name = bm.name();
                else
                    name = QString("%1 (%2)").arg(name).arg(bm.name());
            }
        }

        QStringList sl;
        sl << si->status_name();

        if (si->is_password_needed())
            sl << QObject::tr("Private");
        if (si->get_info("pure", "-1").toInt() == 0)
            sl << QObject::tr("Not pure");

        if ( !si->forbidden_gears().empty() )
        {
            QString text("\nForbidden weapons:\n");
            BOOST_FOREACH( Gear g, si->forbidden_gears() ){
                text += QString( "  %1\n" ).arg( gear(g) );
            }
            sl << text;
        }
        
        QString status = sl.join(", ");

        item->setToolTip(0, status);
        item->setText(1, name);
        item->setIcon(2, geoip::get_flag_by_country(si->country_code));
        item->setToolTip(2, si->country);
        item->setText(3, QString("%1").arg(si->ping, 5));
        item->setText(4, si->mode_name());
        item->setText(5, si->map);

        QString player_count;
        if (si->max_player_count > 0)
            player_count = QString("%1/%2/%3").arg(si->players.size())
            .arg(si->public_slots()).arg(si->max_player_count);

        item->setText(6, player_count);
        item->setToolTip(6, QObject::tr("Current %1 / Public slots %2 / Total %3")
                         .arg(si->players.size()).arg(si->public_slots())
                         .arg(si->max_player_count));
        item->setData(0, c_stamp_role, QVariant::fromValue(si->update_stamp()));
        item->setText(7, id.address());
    }
}
