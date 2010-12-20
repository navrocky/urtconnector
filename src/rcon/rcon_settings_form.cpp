
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_inserter.hpp>

#include <QColorDialog>

#include "ui_rcon_settings_form.h"

#include "rcon_settings.h"
#include "rcon_settings_form.h"


typedef std::map<QWidget*, QWidget*>                Examples;
typedef std::map<QWidget*, rcon_settings::Color>    ColorTypes;

struct rcon_settings_form::Pimpl{

    Pimpl()
    {
        settings.switch_to_tmp();
    }

    rcon_settings settings;
    Ui_rcon_settings_form ui;
    
    Examples   examples;
    ColorTypes types;
};

rcon_settings_form::rcon_settings_form(QWidget* parent, Qt::WindowFlags f)
    : preferences_widget(parent, "Rcon")
    , p_( new Pimpl )
{
    p_->ui.setupUi(this);

    set_icon( QIcon(":/icons/icons/utilities-terminal.png") );
    set_header( tr("Remote console configuration") );

    boost::assign::insert( p_->examples )
        ( p_->ui.info_but,  p_->ui.info_ex    )
        ( p_->ui.inc_but,   p_->ui.inc_ex     )
        ( p_->ui.out_but,   p_->ui.out_ex     )
        ( p_->ui.error_but, p_->ui.err_ex     )
        ( p_->ui.back_but,  p_->ui.background );

    boost::assign::insert( p_->types )
        ( p_->ui.info_but,  rcon_settings::Info       )
        ( p_->ui.inc_but,   rcon_settings::Text       )
        ( p_->ui.out_but,   rcon_settings::Command    )
        ( p_->ui.error_but, rcon_settings::Error      )
        ( p_->ui.back_but,  rcon_settings::Background );

    connect( p_->ui.custom_colors, SIGNAL( clicked(bool) ), SLOT( custom_checked(bool) ) );

    BOOST_FOREACH( const Examples::value_type& p, p_->examples )
        connect( p.first, SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );
}

rcon_settings_form::~rcon_settings_form()
{}

void rcon_settings_form::update_preferences()
{
    p_->ui.custom_colors->setChecked( p_->settings.custom_colors() );

    if( p_->ui.custom_colors->isChecked() )
    {
        QPalette p = p_->ui.background->palette();
        p.setColor( QPalette::Window, p_->settings.color( rcon_settings::Background ) );
        p_->ui.background->setPalette(p);

        p.setColor( QPalette::WindowText, p_->settings.color( rcon_settings::Info ) );
        p_->ui.info_ex->setPalette(p);

        p.setColor( QPalette::WindowText, p_->settings.color( rcon_settings::Text ) );
        p_->ui.inc_ex->setPalette(p);

        p.setColor( QPalette::WindowText, p_->settings.color( rcon_settings::Command ) );
        p_->ui.out_ex->setPalette(p);

        p.setColor( QPalette::WindowText, p_->settings.color( rcon_settings::Error ) );
        p_->ui.err_ex->setPalette(p);
    }
    else
    {
        std::for_each( p_->examples.begin(), p_->examples.end(), boost::bind<void>( &QWidget::setPalette, boost::bind( &Examples::value_type::second, _1 ), palette() ) );
    }
}

void rcon_settings_form::accept()
{
    p_->settings.commit();
    update_preferences();
}

void rcon_settings_form::reject()
{
    p_->settings.restore();
    update_preferences();
}

void rcon_settings_form::reset_defaults()
{
    p_->settings.set_custom_colors(false);
    p_->settings.color( rcon_settings::Background, true );
    p_->settings.color( rcon_settings::Text, true );
    p_->settings.color( rcon_settings::Info, true );
    p_->settings.color( rcon_settings::Command, true );
    p_->settings.color( rcon_settings::Error, true );

    p_->settings.commit();
    
    update_preferences();
}


void rcon_settings_form::custom_checked(bool b)
{
    p_->settings.set_custom_colors( b );
    update_preferences();

    emit changed();
}


void rcon_settings_form::color_clicked()
{
    QWidget* button = qobject_cast< QWidget* >( sender() );
    
    QColor col = p_->settings.color( p_->types[button] );
    col = QColorDialog::getColor(col);
    if (!col.isValid())
        return;
    p_->settings.set_color( p_->types[button], col );
    QPalette p = p_->examples[button]->palette();
    p.setColor( QPalette::WindowText, col );
    p.setColor( QPalette::Window, col );
    p_->examples[button]->setPalette(p);

    emit changed();
}



