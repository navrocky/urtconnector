#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <boost/shared_ptr.hpp>

#include <QDialog>

#include "pointers.h"
#include <common/server_bookmark.h>
#include <jobs/job.h>

class Ui_ServOptsDialogClass;
class geoip;
class job_queue;

class server_options_dialog : public QDialog
{
    Q_OBJECT
public:
    server_options_dialog(QWidget *parent = 0);
    server_options_dialog(QWidget *parent, const server_bookmark& src);

    ~server_options_dialog();

    const server_bookmark& options() const {return opts_;}

    void set_update_params(geoip* gi, job_queue* que);

    void set_server_list(server_list_p val);

public slots:
    void update_name();

protected:
    void accept();

private slots:
    void job_state_changed(job_t::state_t state);
    
private:
    void update_dialog();
    void init();

    boost::shared_ptr<Ui_ServOptsDialogClass> ui_;
    server_bookmark opts_;
    server_id_list ids_;
    server_list_p list_;
    geoip* gi_;
    job_queue* que_;
};

#endif
