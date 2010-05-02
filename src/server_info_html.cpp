#include <memory>
#include <QApplication>
#include <QTextEdit>
#include <QRegExp>
#include <iostream>

#include "server_info_html.h"

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
        "table{margin-left:20px; margin-right:20px; background-color: %2; width: 100%;}"
        ".line1{background-color: %3;}"
        ".line2{background-color: %4;}"
        ".img1{margin-right: 10px;}"
        "</style>");
    return css.arg(window).arg(window).arg(base).arg(alternate);
}

QString plain_to_html(const QString& src)
{
    // TODO Make this more effective. You need to find same function in
    //      the guts of Qt.
    static QTextEdit te;
    static QRegExp rx(">([^>]+)</p></body></html>$");
    te.setPlainText(src);
    if (rx.indexIn(te.toHtml()) >= 0)
        return rx.cap(1);
    else
        return "";
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
        for (player_info_list::const_iterator it = pil.begin(); it != pil.end(); it++)
        {
            players += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td><td>%4</td></tr>")
                    .arg(i % 2 + 1).arg(/*plain_to_html(*/it->nick_name/*)*/).arg(it->ping).arg(it->score);
            i++;
        }
        players += "</table>";
    }

    QString name = plain_to_html(si.name);
    if (name.isEmpty())
        name = qApp->translate("server_info_html", "* Unnamed *");

    QString serv_info;
    QString status_str;
    switch (si.status)
    {
    case server_info::s_none:
        status_str = qApp->translate("server_info_html",
                                     "<img class=\"img1\" src=\":/icons/icons/status-none.png\"> Unknown");
        break;
    case server_info::s_up:
        if ( si.get_info("g_needpass").toInt()  )
        {
            status_str = qApp->translate("server_info_html",
                                         "<img class=\"img1\" src=\":/icons/icons/status-passwd.png\"> Online");
        }
        else
        {
            status_str = qApp->translate("server_info_html",
                                         "<img class=\"img1\" src=\":/icons/icons/status-online.png\"> Online");
        }
        break;
    case server_info::s_updating:
        status_str = qApp->translate("server_info_html",
                                     "<img class=\"img1\" src=\":/icons/icons/status-update.png\"> Updating");
        break;
    case server_info::s_down:
        status_str = qApp->translate("server_info_html",
                                     "<img class=\"img1\" src=\":/icons/icons/status-offline.png\"> Offline");
        break;
    }

    QString country_flag = QString("<img class=\"img1\" src=\"%1\">").arg(si.gi.flag_path(si.id.ip()));
    std::cerr<<"Contfrylag:"<<country_flag.toStdString()<<std::endl;

    serv_info = qApp->translate("server_info_html",
                                "<table width=100%>"
                                "<tr class=\"line1\"><td>Status</td><td>%1</td></tr>"
                                "<tr class=\"line2\"><td>Game mode</td><td>%2</td></tr>"
                                "<tr class=\"line1\"><td>Map</td><td>%3</td></tr>"
                                "<tr class=\"line2\"><td>Ping</td><td>%4</td></tr>"
                                "<tr class=\"line2\"><td>Country</td><td>%5 %6</td></tr>"
                                "<tr class=\"line1\"><td>Max players</td><td>%7</td></tr>"
                                "</table>"
                                )
            .arg(status_str).arg(si.mode_name()).arg(si.map).arg(si.ping).arg(country_flag).arg(si.gi.country( si.id.ip() )).arg(si.max_player_count);

    QString ext_info;
    if (si.info.size() > 0)
    {
        ext_info = qApp->translate("server_info_html", "<hr>Extended info:"
                                   "<table width=100%>"
                                   "<tr class=\"header\"><td>Key</td><td>Value</td></tr>");

        int i = 0;
        for (server_info::info_t::const_iterator it = si.info.begin();
                it != si.info.end(); it++)
        {
            ext_info += QString("<tr class=\"line%1\"><td>%2</td><td>%3</td></tr>")
                    .arg(i % 2 + 1).arg(it->first).arg(/*plain_to_html(*/it->second/*)*/); // plain_to_html dont works with it->second
            i++;
        }
        ext_info += "</table>";
    }

    html = QString("<html><head>%1</head><body><h3>%2</h3>%3<hr>%4%5%6</body></html>")
            .arg(get_css()).arg(name).arg(si.id.address()).arg(serv_info).arg(players)
            .arg(ext_info);
    return html;
}