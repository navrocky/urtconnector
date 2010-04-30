#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

#include "ui_options_dialog.h"
#include "pointers.h"

class options_dialog : public QDialog
{
Q_OBJECT
public:
    options_dialog(QWidget *parent = 0);
    ~options_dialog();

    void set_opts(app_options_p value);
protected:
    void accept();


private slots:
    void choose_binary();
    void choose_qstat_binary();
    void insert_file_path();
    void adv_text_changed(const QString&);
private:
    void update_dialog();

    Ui::OptionsDialogClass ui;
    app_options_p opts_;
};

#endif
