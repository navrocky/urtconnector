
#include <memory>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <QApplication>
#include <QTextEdit>
#include <QRegExp>
#include <iostream>
#include <QtGui/qcolor.h>

#include <common/tools.h>
#include <geoip/geoip.h>
#include "server_info_html.h"
#include "job_update_from_master.h"

using namespace std;
using namespace boost;


QString get_css()
{
    //generic window color
    QString window = QPalette().color(QPalette::Window).name();
    //background color for text entry widgets
    QString base = QPalette().color(QPalette::Base).name();
    //alternate background color in views with alternating row colors
    QString alternate = QPalette().color(QPalette::AlternateBase).name();

    QString css(
        "<style> "
        ".header{background-color: %1;}"
        ".props{margin-left:20px; margin-right:0px; background-color: %2; width: 100%; border-width:0px;}"
        ".line1{background-color: %3;}"
        ".line2{background-color: %4;}"
        ".img1{margin-right: 10px;}"
        ".serv_header{background-color: black; color:white;"
        "padding:5px 10px 5px 10px;font: bold 12pt; font-family:monospace;}"
        "</style>");
    return css.arg(window).arg(window).arg(base).arg(alternate);
}

QString get_server_info_html(const server_info& si)
{
    QString html, players;

    const player_info_list& pil = si.players;

    if (pil.size() > 0)
    {
        players = qApp->translate("server_info_html", "<hr>%1 players:<table width=100%>"
                                  "<tr class=\"header\"><td>Nick</td><td>Ping</td><td>Score</td></tr>").arg(pil.size());
        int i = 0;
        foreach (const player_info& pi, pil)
        {
            players += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td><td>%4</td></tr>")
                       .arg(i % 2 + 1).arg(Qt::escape(pi.nick_name())).arg(pi.ping()).arg(pi.score());
            i++;
        }
        players += "</table>";
    }

    QString name = Qt::escape(si.get_info("sv_hostname"));
    if (name.isEmpty())
        name = si.get_info("hostname");
    if (name.isEmpty())
        name = si.name;
    if ( name.isEmpty() )
        name = qApp->translate("server_info_html", "* Unnamed *");

    static Q3ColorMap html_colors;

    if( html_colors.empty() )
    {
        html_colors = default_colors();
        BOOST_FOREACH( Q3ColorMap::value_type& p, html_colors )
            p.second = p.second.lighter();
    }
    
    name = q3coloring(name, html_colors);
    
    QString serv_info;
    QString status_str;

    int private_slots = si.private_slots();

    if (si.updating)
        status_str = qApp->translate("server_info_html",
                                     "<img class=\"img1\" src=\"icons:status-update.png\"> Updating");
    else
        switch (si.status)
        {
        case server_info::s_none:
            status_str = qApp->translate("server_info_html",
                                        "<img class=\"img1\" src=\"icons:status-none.png\"> Unknown");
            break;
        case server_info::s_up:
            if ( si.is_password_needed()  )
            {
                status_str = qApp->translate("server_info_html",
                                            "<img class=\"img1\" src=\"icons:status-passwd.png\"> Online");
            }
            else
            {
                status_str = qApp->translate("server_info_html",
                                            "<img class=\"img1\" src=\"icons:status-online.png\"> Online");
            }
            if (si.max_player_count > 0 &&
                si.players.size() >= si.max_player_count - private_slots)
                status_str += qApp->translate("server_info_html", ", Full");
            break;
        case server_info::s_down:
            status_str = qApp->translate("server_info_html",
                                        "<img class=\"img1\" src=\"icons:status-offline.png\"> Offline");
            break;
        }



    
    QString country_flag;
    if( !si.country_code.isEmpty() )
         country_flag = QString("<img class=\"img1\" src=\"%1\">")
                 .arg(geoip::get_flag_filename_by_country(si.country_code));

    serv_info = qApp->translate("server_info_html",
                                "<table width=100% class=\"props\">"
                                "<tr class=\"line1\"><td>Status</td><td>%1</td></tr>"
                                "<tr class=\"line2\"><td>Game mode</td><td>%2</td></tr>"
                                "<tr class=\"line1\"><td>Map</td><td>%3</td></tr>"
                                "<tr class=\"line2\"><td>Ping</td><td>%4</td></tr>"
                                "<tr class=\"line1\"><td>Country</td><td>%5 %6</td></tr>"
                                "<tr class=\"line2\"><td>Public slots</td><td>%7</td></tr>"
                                "<tr class=\"line1\"><td>Total slots</td><td>%8</td></tr>"
                                "</table>"
                                )
            .arg(status_str).arg(si.mode_name()).arg(si.map).arg(si.ping)
            .arg(country_flag).arg(si.country).arg(si.max_player_count - private_slots)
            .arg(si.max_player_count);

    QString ext_info;
    if (si.info.size() > 0)
    {
        ext_info = qApp->translate("server_info_html", "<hr>Extended info:"
                                   "<table width=100% class=\"props\">"
                                   "<tr class=\"header\"><td>Key</td><td>Value</td></tr>");

        int i = 0;
        for (server_info::info_t::const_iterator it = si.info.begin();
                it != si.info.end(); it++)
        {
            ext_info += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td></tr>")
                        .arg(i % 2 + 1).arg(it->first).arg(Qt::escape(it->second));
            i++;
        }
        ext_info += "</table>";
    }

    html = QString("<html><head>%1</head><body><table width=100%><tr><td class=\"serv_header\">%2"
            "</td></tr></table>%3<hr>%4%5%6</body></html>")
            .arg(get_css()).arg(name).arg(si.id.address()).arg(serv_info).arg(players)
            .arg(ext_info);
    return html;
}

