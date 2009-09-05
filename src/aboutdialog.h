#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
#include <QDialog>

namespace Ui
{
    class AboutDialogClass;
}

class AboutDialog : public QDialog
{
Q_OBJECT
public:
    AboutDialog(QWidget *parent = 0);
    //AboutDialog(QWidget *parent, const ServerOptions& src);
    ~AboutDialog();

    Ui::AboutDialogClass *ui;

};

#endif // ABOUTDIALOG_H
