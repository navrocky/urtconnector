#include "launch_opts_panel.h"

#include <QFileDialog>
#include <launcher/launcher.h>
#include <common/scoped_tools.h>

#include "ui_launch_opts_panel.h"

launch_opts_panel::launch_opts_panel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::launch_opts_panel)
    , lock_change_(false)
#ifdef Q_OS_UNIX
    , separate_xsession(false)
    , use_mumble_overlay(false)
#endif
{
    ui->setupUi(this);

    connect( ui->select_bin_button,    SIGNAL(clicked()),                 this, SLOT( choose_binary() ));
    connect( ui->insert_file_button,   SIGNAL(clicked()),                 this, SLOT( insert_file_path() ));

    connect( ui->binary_edit,          SIGNAL(textChanged(QString)),      this, SLOT( int_changed() ) );
    connect( ui->adv_cmd_box,          SIGNAL(clicked(bool)),             this, SLOT( int_changed() ) );
    connect( ui->adv_cmd_edit,         SIGNAL(textChanged(QString)),      this, SLOT( int_changed() ) );

    ui->adv_cmd_help_label->setText(tr(
        "<b>%bin%</b> - UrbanTerror binary path<br>"
        "<b>%name%</b> - player name<br>"
        "<b>%pwd%</b> - password<br>"
        "<b>%addr%</b> - hostname or ip and port<br>"
        "<b>%rcon%</b> - RCON password"
    ));
}

launch_opts_panel::~launch_opts_panel()
{
    delete ui;
}

QString launch_opts_panel::binary_path() const
{
    return ui->binary_edit->text();
}

void launch_opts_panel::set_binary_path(const QString& val)
{
    SCOPE_COCK_FLAG(lock_change_);
    ui->binary_edit->setText(val);
}

QString launch_opts_panel::adv_cmd_line() const
{
    return ui->adv_cmd_edit->text();
}

void launch_opts_panel::set_adv_cmd_line(const QString & val)
{
    SCOPE_COCK_FLAG(lock_change_);
    ui->adv_cmd_edit->setText(val);
}

bool launch_opts_panel::use_adv_cmd_line() const
{
    return ui->adv_cmd_box->isChecked();
}

void launch_opts_panel::set_use_adv_cmd_line(bool val)
{
    SCOPE_COCK_FLAG(lock_change_);
    ui->adv_cmd_box->setChecked(val);
}

void launch_opts_panel::set_separate_xsession(bool val)
{
    if (separate_xsession == val)
        return;
    separate_xsession = val;
    update_launch_string();
}

void launch_opts_panel::set_use_mumble_overlay(bool val)
{
    if (use_mumble_overlay == val)
        return;
    use_mumble_overlay = val;
    update_launch_string();
}

void launch_opts_panel::set_mumble_overlay_bin(const QString & val)
{
    if (mumble_overlay_bin == val)
        return;
    mumble_overlay_bin = val;
    update_launch_string();
}

void launch_opts_panel::choose_binary()
{
    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Urban Terror executable"), QString(),
                                              tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fn.isEmpty())
        return;

    ui->binary_edit->setText(fn);
}

void launch_opts_panel::insert_file_path()
{
    QString fn = QFileDialog::getOpenFileName(this,
                                                    tr("Select file to insert"), QString(),
                                                    tr("All Files (*)"));
    if (fn.isEmpty())
        return;

    ui->adv_cmd_edit->insert(fn);
}

void launch_opts_panel::update_launch_string()
{
    launcher l;
    l.set_server_id(server_id("server:12345"));
    l.set_user_name("New_URT_Player");
    l.set_rcon("rcon_pAsSwOrD");
    l.set_password("pAsSwOrD");
    l.set_referee("referee_pAsSwOrD");

#ifdef Q_OS_UNIX
    l.set_mumble_overlay(use_mumble_overlay);
    l.set_mumble_overlay_bin(mumble_overlay_bin);
    QString ls = l.launch_string(ui->adv_cmd_box->isChecked(),
                                 ui->adv_cmd_edit->text(),
                                 ui->binary_edit->text(),
                                 separate_xsession );
#else
    QString ls = l.launch_string(ui->adv_cmd_box->isChecked(),
                                 ui->adv_cmd_edit->text(),
                                 ui->binary_edit->text(),
                                 false );
#endif

    ui->adv_cmd_preview_edit->setText(ls);
}

void launch_opts_panel::int_changed()
{
    update_launch_string();
    if (!lock_change_)
        emit changed();
}
