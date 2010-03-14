#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <QDialog>

#include "ui_servoptsdialog.h"
#include "serveroptions.h"

class ServOptsDialog : public QDialog
{
Q_OBJECT
public:
    ServOptsDialog(QWidget *parent = 0);
    ServOptsDialog(QWidget *parent, const server_options& src);
    ~ServOptsDialog();

    const server_options& options() const {return opts_;}
protected:
    void accept();
private:
    void updateDialog();

    Ui::ServOptsDialogClass ui;
    server_options opts_;
};

#endif
