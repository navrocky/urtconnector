#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <QDialog>
#include "serveroptions.h"

namespace Ui
{
    class ServOptsDialogClass;
}

class ServOptsDialog : public QDialog
{
Q_OBJECT
public:
    ServOptsDialog(QWidget *parent = 0);
    ServOptsDialog(QWidget *parent, const ServerOptions& src);
    ~ServOptsDialog();

    const ServerOptions& options() const {return opts_;}
protected:
    void accept();
private:
    void updateDialog();

    Ui::ServOptsDialogClass *ui;
    ServerOptions opts_;
};

#endif
