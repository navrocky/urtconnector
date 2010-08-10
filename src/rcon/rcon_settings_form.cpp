
#include <QColorDialog>

#include "ui_rcon_settings_form.h"

#include "rcon_settings.h"
#include "rcon_settings_form.h"


typedef std::map<QWidget*, QWidget*> ExamplesMap;
typedef std::map<QWidget*, rcon_settings::Color> ColorsMap;

struct rcon_settings_form::Pimpl{
    Ui_rcon_settings_form ui;

    ExamplesMap e_map;
    ColorsMap c_map;
};

rcon_settings_form::rcon_settings_form(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , p_( new Pimpl )
{
    p_->ui.setupUi(this);

    p_->e_map[p_->ui.info_but] = p_->ui.info_ex;
    p_->e_map[p_->ui.inc_but]  = p_->ui.inc_ex;
    p_->e_map[p_->ui.out_but]  = p_->ui.out_ex;
    p_->e_map[p_->ui.error_but]= p_->ui.err_ex;
    p_->e_map[p_->ui.back_but] = p_->ui.background;

    p_->c_map[p_->ui.info_but] = rcon_settings::Info;
    p_->c_map[p_->ui.inc_but]  = rcon_settings::Text;
    p_->c_map[p_->ui.out_but]  = rcon_settings::Command;
    p_->c_map[p_->ui.error_but]= rcon_settings::Error;
    p_->c_map[p_->ui.back_but] = rcon_settings::Background;

    init();
    
    connect( p_->ui.custom_colors, SIGNAL( clicked(bool) ), SLOT( custom_checked(bool) ) );
    
    connect( p_->ui.info_but, SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );
    connect( p_->ui.inc_but,  SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );
    connect( p_->ui.out_but,  SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );
    connect( p_->ui.error_but,SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );
    connect( p_->ui.back_but, SIGNAL( clicked(bool) ), SLOT( color_clicked() ) );

    connect( p_->ui.defaults, SIGNAL( clicked(bool) ), SLOT( reset_to_defaults() ) );
}

rcon_settings_form::~rcon_settings_form()
{}

void rcon_settings_form::init()
{
    rcon_settings settings;

    p_->ui.custom_colors->setChecked( !settings.adaptive_pallete() );
    
    QPalette p = p_->ui.background->palette();
    p.setColor( QPalette::Window, settings.color( rcon_settings::Background ) );
    p_->ui.background->setPalette(p);

    p.setColor( QPalette::WindowText, settings.color( rcon_settings::Info ) );
    p_->ui.info_ex->setPalette(p);

    p.setColor( QPalette::WindowText, settings.color( rcon_settings::Text ) );
    p_->ui.inc_ex->setPalette(p);

    p.setColor( QPalette::WindowText, settings.color( rcon_settings::Command ) );
    p_->ui.out_ex->setPalette(p);

    p.setColor( QPalette::WindowText, settings.color( rcon_settings::Error ) );
    p_->ui.err_ex->setPalette(p);
}


void rcon_settings_form::custom_checked(bool b)
{
    rcon_settings settings;
    settings.set_adaptive_pallete( !b );
}


void rcon_settings_form::color_clicked()
{
    rcon_settings settings;
    QWidget* s = qobject_cast< QWidget* >( sender() );
    QColorDialog dlg( settings.color( p_->c_map[s] ) );
    if ( dlg.exec() ==  QDialog::Accepted )
    {
        settings.set_color( p_->c_map[s], dlg.selectedColor() );
        QPalette p = p_->e_map[s]->palette();
        p.setColor( QPalette::WindowText, dlg.selectedColor() );
        p.setColor( QPalette::Window, dlg.selectedColor() );
        p_->e_map[s]->setPalette(p);
    }

}

void rcon_settings_form::reset_to_defaults()
{
    rcon_settings settings;
    settings.color( rcon_settings::Background, true );
    settings.color( rcon_settings::Text, true );
    settings.color( rcon_settings::Info, true );
    settings.color( rcon_settings::Command, true );
    settings.color( rcon_settings::Error, true );
    init();
}


