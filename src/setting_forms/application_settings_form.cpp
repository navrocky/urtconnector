
#include <QFileDialog>
#include <QLibraryInfo>
#include <QTranslator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>

#include <common/scoped_tools.h>
#include <common/exception.h>
#include <common/tools.h>
#include <common/qstat_options.h>
#include <common/play_sound.h>
#include "app_options.h"
#include <config.h>

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

    ret << CountryId( "English", "" );

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

    connect(p_->ui.style_sheet_refresh_button, SIGNAL(clicked()), SLOT(apply_style_sheet()));
    QToolButton* tb = p_->ui.style_sheet_file_edit->addButton();
    tb->setIcon(QIcon("icons:choose-file.png"));
    tb->setToolTip(tr("Select a style sheet file name"));
    connect(tb, SIGNAL(clicked()), SLOT(select_css_file()));

    tb = p_->ui.notify_sound_edit->addButton();
    tb->setIcon(QIcon("icons:choose-file.png"));
    tb->setToolTip(tr("Select notification sound file name"));
    connect(tb, SIGNAL(clicked()), SLOT(select_sound_file()));
    connect(p_->ui.play_btn, SIGNAL(clicked()), SLOT(play_sound_file()));

    connect(p_->ui.hide_mainwindow_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.holiday_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.language_box, SIGNAL(currentIndexChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.style_box, SIGNAL(currentIndexChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.group_clipboard_watch, SIGNAL(clicked(bool)), this, SLOT(int_changed()));
    connect(p_->ui.clip_regexp_edit, SIGNAL(textChanged(const QString&)), this, SLOT(int_changed()));
    connect(p_->ui.clip_host_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_port_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_password_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.style_sheet_file_edit, SIGNAL(textChanged(const QString&)), this, SLOT(int_changed()));
    connect(p_->ui.notify_sound_edit, SIGNAL(textChanged(const QString&)), this, SLOT(int_changed()));
}

void application_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void application_settings_form::update_preferences()
{
    SCOPE_COCK_FLAG(lock_change_);

    app_settings as;
    clip_settings cs;
    p_->ui.hide_mainwindow_check->setChecked(as.start_hidden());
    p_->ui.holiday_check->setChecked(as.use_holiday_mode());

    const QString country_code = as.country_name();

    p_->ui.language_box->clear();
    foreach( const CountryId& cid, find_countries() ){
        boost::shared_ptr<QTranslator> local = local_translator( cid.second );
        QString text = local->translate( "language", cid.first.toLocal8Bit() );
        if( text.isEmpty() )
            text = cid.first;
        p_->ui.language_box->addItem( text, cid.second );
    }
    p_->ui.language_box->setCurrentIndex( p_->ui.language_box->findData( country_code ) );

    const QString style_name = as.style_name();

    p_->ui.style_box->clear();
    p_->ui.style_box->addItem(tr("Default"), as.default_style_name());
    foreach( const QString& style, QStyleFactory::keys() ){
        p_->ui.style_box->addItem( style, style );
    }
    p_->ui.style_box->setCurrentIndex( p_->ui.style_box->findData( style_name ) );
    
    p_->ui.group_clipboard_watch->setChecked(cs.watching());
    p_->ui.clip_regexp_edit->setText(cs.regexp());
    p_->ui.clip_host_spin->setValue(cs.host());
    p_->ui.clip_port_spin->setValue(cs.port());
    p_->ui.clip_password_spin->setValue(cs.password());
    p_->ui.style_sheet_file_edit->setText(as.style_sheet_file());
    p_->ui.notify_sound_edit->setText(as.notification_sound());
}

void application_settings_form::accept()
{
    app_settings as;

    as.start_hidden_set(p_->ui.hide_mainwindow_check->isChecked());
    as.use_holiday_mode_set(p_->ui.holiday_check->isChecked());

    const QString country_code =  p_->ui.language_box->itemData( p_->ui.language_box->currentIndex() ).toString();
    const QString style_name =  p_->ui.style_box->itemData( p_->ui.style_box->currentIndex() ).toString();

    if( country_code != as.country_name() || style_name != as.style_name() )
        QMessageBox::information(this, tr("Language/Style preferences"),
                                 tr("Selected language/style will be applied after restart"));

    as.country_name_set( country_code );
    as.style_name_set( style_name );
    QApplication::setStyle( style_name );
    
    as.style_sheet_file_set(p_->ui.style_sheet_file_edit->text());
    as.notification_sound_set(p_->ui.notify_sound_edit->text());

    clip_settings cs;
    cs.watching_set(p_->ui.group_clipboard_watch->isChecked());
    cs.regexp_set(p_->ui.clip_regexp_edit->text());
    cs.host_set(p_->ui.clip_host_spin->value());
    cs.port_set(p_->ui.clip_port_spin->value());
    cs.password_set(p_->ui.clip_password_spin->value());

    apply_style_sheet();
}

void application_settings_form::reject()
{
    update_preferences();
}

void application_settings_form::reset_defaults()
{
    app_settings as;
    as.start_hidden_reset();
    as.use_holiday_mode_reset();
    as.country_name_reset();
    as.style_sheet_file_reset();
    as.notification_sound_reset();

    clip_settings cs;
    cs.watching_reset();
    cs.regexp_reset();
    cs.host_reset();
    cs.password_reset();
    cs.port_reset();

    update_preferences();
    apply_style_sheet();
}

void application_settings_form::apply_style_sheet()
{
    QString fn = p_->ui.style_sheet_file_edit->text();
    load_app_style_sheet(fn);
}

void application_settings_form::select_css_file()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Select a style sheet file"),
                                              QString(), tr("CSS Files (*.css)(*.css);;All files(*)"));
    if (!fn.isEmpty())
        p_->ui.style_sheet_file_edit->setText(fn);
}

void application_settings_form::select_sound_file()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Select a notification sound"),
                                              QString(), tr("Sounds (*.ogg *.wav)(*.ogg *.wav);;All files(*)"));
    if (!fn.isEmpty())
        p_->ui.notify_sound_edit->setText(fn);
}

void application_settings_form::play_sound_file()
{
    play_sound(p_->ui.notify_sound_edit->text());
}
