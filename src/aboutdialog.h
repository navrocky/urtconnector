#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui
{
class AboutDialogClass;
}

class about_dialog : public QDialog
{
    Q_OBJECT
public:
    about_dialog(QWidget *parent = 0);
    ~about_dialog();
private:
    std::auto_ptr<Ui::AboutDialogClass> ui_;
};

#endif // ABOUTDIALOG_H
