#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <memory>

#include <QDialog>

#include "pointers.h"
#include "server_options.h"
#include "jobs/job.h"

class Ui_ServOptsDialogClass;
class geoip;
class qstat_options;
class job_queue;

class server_options_dialog : public QDialog
{
    Q_OBJECT
public:
    server_options_dialog(QWidget *parent = 0);
    server_options_dialog(QWidget *parent, const server_options& src);

    const server_options& options() const {return opts_;}

    void set_update_params(geoip* gi, qstat_options* opts, job_queue* que);

protected:
    void accept();

private slots:
    void update_name();
    void job_state_changed(job_t::state_t state);
    
private:
    void update_dialog();
    void init();

    std::auto_ptr<Ui_ServOptsDialogClass> ui_;
    server_options opts_;
    server_id_list ids_;
    server_list_p list_;
    geoip* gi_;
    qstat_options* qstat_opts_;
    job_queue* que_;
};

#endif
