#include <QPalette>

#include "config.h"

#include "ui_about_dialog.h"
#include "about_dialog.h"

QString prepare_html(const QString& s)
{
    QString html(
        "<html><head><style> "
        ".name{margin-bottom:0px;}"
        ".info{margin-left:20px; margin-top:0px;}"
        "</style></head><body>%1</body></html>");
    return html.arg(s);
}

about_dialog::about_dialog(QWidget *parent)
: QDialog(parent)
, ui_(new Ui::AboutDialogClass)
{
    ui_->setupUi(this);
    ui_->label_version->setText(tr("version %1 \"%2\"").arg(URT_VERSION).arg(URT_CODENAME));

    QPalette transp_palette;
    transp_palette.setColor(QPalette::Base, Qt::transparent);
    transp_palette.setColor(QPalette::Text, transp_palette.color(QPalette::WindowText));

    ui_->about_browser->setPalette(transp_palette);
    ui_->about_browser->setHtml(prepare_html(tr(
        "<p>UrTConnector intended for help in everyday life of professional "
        "<a href=\"http://www.urbanterror.net\">UrbanTerror</a> player.</p>"
        "<p>Search for game servers, players, online clans, bookmarks, "
        "miscellaneous information about servers, quick connect to server "
        "and many other features you will find in this application.</p>"
        "<p>Project page <a href=\"http://code.google.com/p/urtconnector\">"
        "http://code.google.com/p/urtconnector</a></p>"
        "<p>License: <a href=\"http://www.gnu.org/licenses/gpl.html\">"
        "GNU General Public License Version 3</a></p>"
    )));
    ui_->developers_browser->setPalette(transp_palette);
    ui_->developers_browser->setHtml(prepare_html(tr(
        "<p>Developed by members of the best russian clan "
        "<a href=\"http://www.urt-xaoc.ru\">=XaoC=</a>.</p>"
        "<p class=\"name\">Navrocky Vladislav (=XaoC=vlad.ru)<p>"
        "<p class=\"info\"><a href=\"mailto:navrocky@visualdata.ru\">navrocky@visualdata.ru</a><br>"
        "Project leader, maintainer, architecture, windows and rpm packager.</p>"
        "<p class=\"name\">EnD3n1s<p>"
        "<p class=\"info\"><a href=\"mailto:endenis@mail.ru\">endenis@mail.ru</a><br>"
        "Founder, base concept, first release, tester.</p>"
        "<p class=\"name\">Samoilenko Yuri (jerry.gkb.ru)<p>"
        "<p class=\"info\"><a href=\"mailto:kinallru@gmail.com\">kinallru@gmail.com</a><br>"
        "Code, ideas, Gentoo packager.</p>"
    )));
    ui_->thanks_browser->setPalette(transp_palette);
    ui_->thanks_browser->setHtml(prepare_html(tr(
        "<p class=\"name\">Tsyss Dmitry (=XaoC=Dmitro)<p>"
        "<p class=\"info\"><a href=\"mailto:dtsyss@gmail.com\">dtsyss@gmail.com</a><br>"
        "Clan leader, help on project, tester.</p>"
    )));
    ui_->translation_browser->setPalette(transp_palette);
    ui_->translation_browser->setHtml(prepare_html(tr(
        "<p>UrTConnector translated to many languages. "
        "Thanks to people who helped with translation.</p>"
        "<p>If you want help with translation UrTConnector to your language - "
        "<a href=\"http://code.google.com/p/urtconnector\">join</a> us!</p>"
    )));
}

about_dialog::~about_dialog()
{
}
