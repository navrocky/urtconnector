#ifndef SERVOPTSDIALOG_H
#define SERVOPTSDIALOG_H

#include <QDialog>

namespace Ui
{
    class ServOptsDialogClass;
}

class ServOptsDialog : public QDialog
{
Q_OBJECT
public:
    ServOptsDialog(QWidget *parent = 0);
    ~ServOptsDialog();
private:
    Ui::ServOptsDialogClass *ui;
};

#endif
