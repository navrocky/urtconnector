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

    bool christmas_mode() const {return christmas_mode_;}
    void set_christmas_mode(bool val);

private:
    void update_christmas_mode();

    std::auto_ptr<Ui::AboutDialogClass> ui_;
    bool christmas_mode_;
};

#endif // ABOUTDIALOG_H
