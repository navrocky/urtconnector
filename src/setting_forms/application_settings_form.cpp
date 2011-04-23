
#include <QFileDialog>
#include <QLibraryInfo>
#include <QTranslator>
#include <QMessageBox>

#include <common/scoped_tools.h>
#include "app_options.h"
#include "qstat_options.h"

#include "ui_application_settings_form.h"
#include "application_settings_form.h"

typedef std::pair<QString, QString> CountryId;
typedef QList<CountryId> Countries;

namespace {

//extract name of country from filename: "en" from "qt_en.qm"
QString country_code_from_qm( const QString& qm_filename ){
    return qm_filename.mid( qm_filename.length() - 5, 2 );
}

QString country_name_from_code( const QString& code  ){
    return QLocale::languageToString( QLocale(code).language() );
}

//find country codes for existing translation files in qt distribution
Countries find_countries()
{
    Countries ret;
    
    QDir dir( QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    
    //find all translation files in qt standard path
    foreach( const QString& file, dir.entryList( QStringList("qt_[a-zA-Z0-9][a-zA-Z0-9].qm"), QDir::Files, QDir::Name ) ){
        ret << CountryId(
            country_name_from_code( country_code_from_qm( file ) ),
            country_code_from_qm( file )
        );
    }
    
    return ret;
}

}


struct application_settings_form::Pimpl
{
    Ui_application_settings_form ui;
};

application_settings_form::application_settings_form(QWidget* parent)
: preferences_widget(parent, tr("Application"))
, p_(new Pimpl)
, lock_change_(false)
{
    p_->ui.setupUi(this);

    set_icon(QIcon("icons:configure.png"));
    set_header(tr("Application settings"));

    connect(p_->ui.hide_mainwindow_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.holiday_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.language_box, SIGNAL(currentIndexChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.group_clipboard_watch, SIGNAL(clicked(bool)), this, SLOT(int_changed()));
    connect(p_->ui.clip_regexp_edit, SIGNAL(textChanged(const QString&)), this, SLOT(int_changed()));
    connect(p_->ui.clip_host_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_port_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_password_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
}

void application_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void application_settings_form::update_preferences()
{
    scoped_value_change<bool> s(lock_change_, true, false);

    app_settings as;
    clip_settings cs;
    p_->ui.hide_mainwindow_check->setChecked(as.start_hidden());
    p_->ui.holiday_check->setChecked(as.use_holiday_mode());

    QString country_code = as.country_name();

    p_->ui.language_box->clear();
    foreach( const CountryId& cid, find_countries() ){
        boost::shared_ptr<QTranslator> local = local_translator( cid.second );
        QString text = local->translate( "language", cid.first.toLocal8Bit() );
        if( text.isEmpty() )
            text = cid.first;
        p_->ui.language_box->addItem( text, cid.second );
    }
    p_->ui.language_box->setCurrentIndex( p_->ui.language_box->findData( country_code ) );

    p_->ui.group_clipboard_watch->setChecked(cs.watching());
    p_->ui.clip_regexp_edit->setText(cs.regexp());
    p_->ui.clip_host_spin->setValue(cs.host());
    p_->ui.clip_port_spin->setValue(cs.port());
    p_->ui.clip_password_spin->setValue(cs.password());
}

void application_settings_form::accept()
{
    app_settings as;
    clip_settings cs;

    as.set_start_hidden(p_->ui.hide_mainwindow_check->isChecked());
    as.set_holiday_mode(p_->ui.holiday_check->isChecked());
    
    QString country_code =  p_->ui.language_box->itemData( p_->ui.language_box->currentIndex() ).toString();
    
    if( country_code != as.country_name() )
        QMessageBox::information(this, tr("Language preferences"), tr("Selected language will applyed after restart"));
    
    as.set_country_name( country_code );
    
    cs.set_watching(p_->ui.group_clipboard_watch->isChecked());
    cs.set_regexp(p_->ui.clip_regexp_edit->text());
    cs.set_host(p_->ui.clip_host_spin->value());
    cs.set_port(p_->ui.clip_port_spin->value());
    cs.set_password(p_->ui.clip_password_spin->value());
}

void application_settings_form::reject()
{
    update_preferences();
}

void application_settings_form::reset_defaults()
{
    clip_settings().reset_regexp();

    update_preferences();
}
