#include "about_dialog.h"

#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>

#include <QPalette>
#include <QPainter>
#include <QTimer>

#include "config.h"
#include "app_options.h"
#include "ui_about_dialog.h"

QString prepare_html(const QString& s)
{
    QString html("<html><head><style> "
                 ".name{margin-bottom:0px;font:bold;}"
                 ".info{margin-left:20px; margin-top:0px;}"
                 "</style></head><body>%1</body></html>");
    return html.arg(s);
}

about_dialog::about_dialog(QWidget *parent)
: QDialog(parent)
, ui_(new Ui::AboutDialogClass)
{
    srand(std::time(NULL));

    if (is_christmas_mode())
        new blizzard(this);

    ui_->setupUi(this);

    QString build = URT_BUILD;
    QString version;
    if (!build.isEmpty())
        version = tr("version %1 \"%2\" rev. %3").arg(URT_VERSION).arg(URT_CODENAME).arg(URT_BUILD);
    else
        version = tr("version %1 \"%2\"").arg(URT_VERSION).arg(URT_CODENAME);

    ui_->label_version->setText(version);
    ui_->logo_label->setPixmap(QPixmap("images:logo.png"));

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
                                                     "<p class=\"name\">Konstantin J. Chernov<p>"
                                                     //        "<p class=\"info\"><a href=\"mailto:kinallru@gmail.com\">kinallru@gmail.com</a><br>"
                                                     "<p class=\"info\">Fast SQLite code.</p>"
                                                     )));
    ui_->thanks_browser->setPalette(transp_palette);
    ui_->thanks_browser->setHtml(prepare_html(tr(
                                                 "<p class=\"name\">Tsyss Dmitry (Dmitro)<p>"
                                                 "<p class=\"info\"><a href=\"mailto:dtsyss@gmail.com\">dtsyss@gmail.com</a><br>"
                                                 "Clan leader, help on project, tester.</p>"
                                                 "<p class=\"name\">Stanislav Nikitin (p0z1tr0n)<p>"
                                                 "<p class=\"info\"><a href=\"mailto:pozitpoh@gmail.com\">pozitpoh@gmail.com</a><br>"
                                                 "Contributor, help on project, tester, ideas generator.</p>"
                                                 )));
    ui_->translation_browser->setPalette(transp_palette);
    ui_->translation_browser->setHtml(prepare_html(tr(
                                                      "<p class=\"name\">Vladislav Navrocky (=XaoC=vlad.ru)<p>"
                                                      "<p class=\"info\"><a href=\"mailto:navrocky.vlad@gmail.com\">navrocky.vlad@gmail.com</a><br>"
                                                      "Russian translation.</p>"
                                                      "<p class=\"name\">MajkiFajki<p>"
                                                      "<p class=\"info\"><a href=\"mailto:MajkFajkiPublic@gmail.com\">MajkFajkiPublic@gmail.com</a><br>"
                                                      "Polish translation.</p>"
                                                      "<p>UrTConnector translated to many languages. "
                                                      "Thanks to people who helped with translation.</p>"
                                                      "<p>If you want to help with translation UrTConnector to your language - "
                                                      "<a href=\"http://code.google.com/p/urtconnector\">join</a> us!</p>"
                                                      )));
}

snowflake::snowflake()
: init_(false)
{
}

snowflake::snowflake(const QPointF& pos, const QPixmap& pm)
: pos_(pos)
, init_(true)
{
    size_ = rand() % 20 + 5;
    pm_ = pm.scaled(size_, size_, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    //this is magic numbers
    int max_v_speed = 15;
    int min_v_speed = 4;
    int max_h_speed = 5;

    vspeed_ = std::max((rand() % max_v_speed + min_v_speed) / 5.0, 1.0);
    hspeed_ = (rand() % max_h_speed) / 2.5;
    rotate_speed_ = (float(rand() % 10000) / 10000.0) * 2.0 - 1.0;

    if (rand() % 2 < 1) hspeed_ = -hspeed_;
}

void snowflake::tick()
{
    pos_.setX(pos_.x() + std::min(hspeed_, size_ / (float) 2.0));
    pos_.setY(pos_.y() + std::min(vspeed_, size_ / (float) 2.0));
    rotate_ += rotate_speed_;
}

bool snowflake::is_ok() const
{
    return init_;
}

const QPixmap& snowflake::pixmap() const
{
    return pm_;
}

////////////////////////////////////////////////////////////////////////////////
// blizzard

blizzard::blizzard(QWidget* w)
: QObject(w)
, w_(w)
, flakes_(30)
{
    w->installEventFilter(this);
    QTimer* t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SLOT(animate()));
    t->start(50);

    QPixmap im("images:snowflake.png");

    prototype_ = QPixmap(im.size());
    prototype_.fill(Qt::transparent);

    QPainter(&prototype_).drawPixmap(0, 0, im);
}

void blizzard::animate()
{
    BOOST_FOREACH(snowflake& sf, flakes_)
    {
        sf.tick();
    }
    w_->update();
}

bool blizzard::eventFilter(QObject* o, QEvent* e)
{
    bool ret = QObject::eventFilter(o, e);

    if (o != w_ || (o == w_ && e->type() != QEvent::Paint))
        return ret;

    BOOST_FOREACH(snowflake& sf, flakes_)
    {

        if (!sf.is_ok())
            sf = snowflake(QPointF(rand() % w_->width(), rand() % w_->height()), prototype_);

        QPainter p(w_);

        p.translate(sf.pos());
        p.rotate(sf.rotate());

        p.drawPixmap(-sf.size() / 2, -sf.size() / 2, sf.pixmap());

        if (sf.pos().y() >= w_->height() || sf.pos().x() >= w_->width())
            sf = snowflake(QPointF(rand() % w_->width(), 0), prototype_);
    }

    return ret;
}


