
#include <common/exception.h>
#include <common/server_list.h>
#include "server_options_dialog.h"
#include "job_update_selected.h"
#include "jobs/job_queue.h"
#include "ui_server_options_dialog.h"

server_options_dialog::server_options_dialog(QWidget *parent)
: QDialog(parent)
, gi_(NULL)
, qstat_opts_(NULL)
, que_(NULL)
{
    init();
    setWindowTitle(tr("Add new server favorite"));
}

server_options_dialog::server_options_dialog(QWidget * parent, const server_bookmark & src)
: QDialog(parent)
, opts_(src)
, gi_(NULL)
, qstat_opts_(NULL)
, que_(NULL)
{
    init();
    setWindowTitle(tr("Server favorite options"));
}

server_options_dialog::~server_options_dialog()
{}


void server_options_dialog::init()
{
    ui_.reset(new Ui_ServOptsDialogClass);
    ui_->setupUi(this);
    update_dialog();

    connect(ui_->update_name_button, SIGNAL(clicked()), SLOT(update_name()));
}

void server_options_dialog::accept()
{
    server_id id(ui_->addressEdit->text().trimmed());
    if (id.is_empty())
        throw qexception(tr("Server address must be non empty"));

    opts_.set_id(id);
    opts_.set_name(ui_->nameEdit->text().trimmed());
    opts_.set_password(ui_->passwordEdit->text().trimmed());
    opts_.set_rcon_password(ui_->rconEdit->text().trimmed());
    opts_.set_ref_password(ui_->refEdit->text().trimmed());
    opts_.set_comment(ui_->commentEdit->toPlainText().trimmed());
    QDialog::accept();
}

void server_options_dialog::update_dialog()
{
    if (!(opts_.id().is_empty()))
        ui_->addressEdit->setText(opts_.id().address());
    else
        ui_->addressEdit->setText(QString());

    ui_->nameEdit->setText(opts_.name());
    ui_->passwordEdit->setText(opts_.password());
    ui_->rconEdit->setText(opts_.rcon_password());
    ui_->refEdit->setText(opts_.ref_password());
    ui_->commentEdit->setPlainText(opts_.comment());
}

void server_options_dialog::set_update_params ( geoip* gi, qstat_options* opts, job_queue* que)
{
    gi_ = gi;
    qstat_opts_ = opts;
    que_ = que;
}

void server_options_dialog::job_state_changed ( job_t::state_t state )
{
    if (state == job_t::js_finished)
    {
        const server_id& id = ids_.front();
        const server_info_list& list = list_->list();
        server_info_list::const_iterator it = list.find(id);
        if (it != list.end()) 
            ui_->nameEdit->setText(it->second->name);
        ui_->update_name_button->setEnabled(true);
    } else
    if (state == job_t::js_canceled)
        ui_->update_name_button->setEnabled(true);
}

void server_options_dialog::update_name()
{
    server_id id(ui_->addressEdit->text().trimmed());
    ids_.clear();
    ids_.push_back(id);
    if (!list_)
        list_.reset( new server_list );
    job_p job(new job_update_selected(ids_, list_, *gi_, qstat_opts_));
    que_->add_job(job);
    connect(job.get(), SIGNAL(state_changed(job_t::state_t)), SLOT(job_state_changed(job_t::state_t)));
    ui_->update_name_button->setEnabled(false);
}

void server_options_dialog::set_server_list(server_list_p val)
{
    list_ = val;
}



