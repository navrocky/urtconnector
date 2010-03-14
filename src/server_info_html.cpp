#include <QApplication>

#include "server_info_html.h"

QString get_css() {
    return "<style> "
    ".header{background-color: #DCDCDC;}"
    "table{margin-left:20px; background-color: #EEEEEE; width: 100%;}"
    ".line1{background-color: #EEEEEE;}"
    ".line2{background-color: #FFFFFF;}"
    "</style>";
}

QString get_server_info_html(const server_info& si) {
    QString html, players;

    const player_info_list& pil = si.players;

    if (pil.size() > 0) {
        players = qApp->translate("server_info_html", "<hr>%1 players:<table width=100%>"
                "<tr class=\"header\"><td>Nick</td><td>Ping</td><td>Score</td></tr>").arg(pil.size());
        int i = 0;
        for (player_info_list::const_iterator it = pil.begin(); it != pil.end(); it++)
        {
            players += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td><td>%4</td></tr>")
            .arg(i % 2 + 1).arg(it->nick_name).arg(it->ping).arg(it->score);
            i++;
        }
        players += "</table>";
    }

    QString name = si.name;
    if (name.isEmpty())
        name = qApp->translate("server_info_html", "* Unnamed *");

    QString serv_info;
    if (si.status == server_info::Up)
    {
        serv_info = qApp->translate("server_info_html",
            "<table width=100%>"
            "<tr class=\"line1\"><td>Game mode</td><td>%1</td></tr>"
            "<tr class=\"line2\"><td>Map</td><td>%2</td></tr>"
            "<tr class=\"line1\"><td>Ping</td><td>%3</td></tr>"
            //"<tr class=\"line1\"><td>Ping</td><td>%3</td></tr>"
            "<tr class=\"line2\"><td>Max players count</td><td>%4</td></tr>"
            "</table>"
            )
            .arg(si.modeName()).arg(si.map).arg(si.ping).arg(si.maxPlayerCount);
    } else
    {
        serv_info = qApp->translate("server_info_html", "<table width=100%>"
            "<tr class=\"line1\"><td>Status</td><td>Offline</td></tr></table>");
    }

    QString ext_info;
    if (si.info.size() > 0) {
        ext_info = qApp->translate("server_info_html", "<hr>Extended info:"
                "<table width=100%>"
                "<tr class=\"header\"><td>Key</td><td>Value</td></tr>");

        int i = 0;
        for (server_info::Info::const_iterator it = si.info.begin();
                it != si.info.end(); it++) {
            ext_info += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td></tr>")
                    .arg(i % 2 + 1).arg(it->first).arg(it->second);
            i++;
        }
        ext_info += "</table>";
    }

    html = QString("<html><head>%1</head><body><h3>%2</h3>%3<hr>%4%5%6</body></html>")
            .arg(get_css()).arg(name).arg(si.id.address()).arg(serv_info).arg(players)
            .arg(ext_info);
    return html;
}