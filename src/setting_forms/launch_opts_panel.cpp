#include "launch_opts_panel.h"

#include <QFileDialog>
#include <QComboBox>
#include <launcher/launcher.h>
#include <common/scoped_tools.h>

#include "ui_launch_opts_panel.h"

launch_opts_panel::launch_opts_panel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::launch_opts_panel)
    , lock_change_(false)
#ifdef Q_OS_UNIX
    , separate_xsession_(false)
    , use_mumble_overlay_(false)
#endif
{
    ui->setupUi(this);

    connect( ui->select_bin_button,    SIGNAL(clicked()),                 this, SLOT( choose_binary() ));
    connect( ui->insert_file_button,   SIGNAL(clicked()),                 this, SLOT( insert_file_path() ));

    connect( ui->binary_edit,          SIGNAL(textChanged(QString)),      this, SLOT( int_changed() ) );
    connect( ui->adv_cmd_box,          SIGNAL(clicked(bool)),             this, SLOT( int_changed() ) );
    connect( ui->adv_cmd_edit,         SIGNAL(textChanged(QString)),      this, SLOT( int_changed() ) );
    connect( ui->client_version_combo, SIGNAL(currentIndexChanged(int)),  this, SLOT( int_changed() ) );

    ui->adv_cmd_help_label->setText(tr(
        "<b>%bin%</b> - UrbanTerror binary path<br>"
        "<b>%name%</b> - player name<br>"
        "<b>%pwd%</b> - password<br>"
        "<b>%addr%</b> - hostname or ip and port<br>"
        "<b>%rcon%</b> - RCON password"
    ));
    set_new_client(false);
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
    if (separate_xsession_ == val)
        return;
    separate_xsession_ = val;
    update_launch_string();
}

void launch_opts_panel::set_use_mumble_overlay(bool val)
{
    if (use_mumble_overlay_ == val)
        return;
    use_mumble_overlay_ = val;
    update_launch_string();
}

void launch_opts_panel::set_mumble_overlay_bin(const QString & val)
{
    if (mumble_overlay_bin_ == val)
        return;
    mumble_overlay_bin_ = val;
    update_launch_string();
}

void launch_opts_panel::set_new_client(bool val)
{
    ui->client_version_label->setVisible(val);
    ui->client_version_combo->setVisible(val);

    if (val)
    {
        QComboBox* combo = ui->client_version_combo;
        for (int i = 1; i < 99; i++)
        {
            QString s = QString("4.2.%1").arg(i, 3, 10, QChar('0'));
            combo->addItem(s, s);
        }
    }
}

QString launch_opts_panel::client_version() const
{
    QComboBox* combo = ui->client_version_combo;
    QString s = combo->itemData(combo->currentIndex()).toString();
    return s;
}

void launch_opts_panel::set_client_version(const QString& s)
{
    QComboBox* combo = ui->client_version_combo;
    int i = 0;
    if (!s.isEmpty())
        i = combo->findData(s);
    combo->setCurrentIndex(i);
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
    l.set_mumble_overlay(use_mumble_overlay_);
    l.set_mumble_overlay_bin(mumble_overlay_bin_);
    QString ls = l.launch_string(ui->adv_cmd_box->isChecked(),
                                 ui->adv_cmd_edit->text(),
                                 ui->binary_edit->text(),
                                 separate_xsession_ );
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
