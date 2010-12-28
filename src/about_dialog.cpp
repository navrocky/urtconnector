
#include <ctime>

#include <boost/foreach.hpp>

#include <QPalette>
#include <QPainter>
#include <QTimer>

#include "config.h"
#include "app_options.h"

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
    srandom( std::time(NULL) );

    if( app_settings().christmas_mode() )
        new blizzard(this);

    ui_->setupUi(this);
    ui_->label_version->setText(tr("version %1 \"%2\"").arg(URT_VERSION).arg(URT_CODENAME));
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




snowflake::snowflake()
    : init_(false)
{}


snowflake::snowflake(int x, const QPixmap& pm)
    : x_(x)
    , y_(0)
    , init_(true)
{
    size = random() % 10 + 4;
    pm_ = pm.scaled( size, size );

    //this is magic numbers
    int max_v_speed = 15;
    int min_v_speed = 4;

    int max_h_speed = 5;

    vspeed_ = std::max( ( random() % max_v_speed + min_v_speed ) / 5.0, 1.0);
    hspeed_ = ( random() % max_h_speed ) /2.5;

    if( random() % 2 < 1) hspeed_ = -hspeed_;
}

void snowflake::tick()
{
    x_ += std::min( hspeed_, size / (float)2.0 );
    y_ += std::min( vspeed_, size / (float)2.0 );
}

int snowflake::x() const
{ return x_; }

int snowflake::y() const
{ return y_; }

bool snowflake::is_ok() const
{ return init_; }

const QPixmap& snowflake::pixmap() const
{ return pm_; }





blizzard::blizzard(QWidget* w)
    : QObject(w)
    , w_(w)
    , flakes_(15)
{
    w->installEventFilter( this );
    QTimer* t = new QTimer(this);

    connect( t, SIGNAL( timeout() ), w, SLOT( update()) );
    t->start(50);

    QImage im( "images:snowflake.png" );

    for ( int i = 0; i< im.width(); ++i )
    {
        for (int j = 0; j< im.height(); ++j)
        {
            int index = im.pixelIndex(i,j);
            QRgb color = im.color(index);
            im.setColor( index, qRgba( qRed(color), qGreen(color), qBlue(color), std::min(150, qAlpha(color) ) ));
        }
    }

    prototype = QPixmap( im.size() );
    prototype.fill( Qt::transparent );

    QPainter( &prototype ).drawImage(0, 0, im);
}


bool blizzard::eventFilter(QObject* o, QEvent* e)
{
    bool ret = QObject::eventFilter(o, e);

    if ( o != w_ || ( o == w_ && e->type() != QEvent::Paint ) )
        return ret;

    QPainter p(w_);

    BOOST_FOREACH( snowflake& sf, flakes_ ){

        if ( !sf.is_ok() )
            sf = snowflake( random() % w_->width(), prototype );

        sf.tick();

        p.drawPixmap( sf.x(), sf.y(), sf.pixmap() );

        if( sf.y() >= w_->height() || sf.x() >= w_->width() )
            sf = snowflake();
    }

    return ret;
}


