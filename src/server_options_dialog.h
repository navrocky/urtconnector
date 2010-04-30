#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <QDialog>

#include "ui_server_options_dialog.h"
#include "server_options.h"

class server_options_dialog : public QDialog
{
Q_OBJECT
public:
    server_options_dialog(QWidget *parent = 0);
    server_options_dialog(QWidget *parent, const server_options& src);

    const server_options& options() const {return opts_;}
protected:
    void accept();
private:
    void update_dialog();

    Ui::ServOptsDialogClass ui;
    server_options opts_;
};

#endif
