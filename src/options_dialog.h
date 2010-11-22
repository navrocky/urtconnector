#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

#include "ui_options_dialog.h"
#include "pointers.h"

namespace anticheat
{
class settings_widget;
}

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
    void choose_geoip_database();
    void insert_file_path();
    void adv_text_changed(const QString&);
    void x_check();
private:
    void update_dialog();

    Ui::OptionsDialogClass ui;
    app_options_p opts_;
    anticheat::settings_widget* anticheat_;
};

class dialog_syncer: public QObject
{
    Q_OBJECT
public:
    dialog_syncer(){};
    ~dialog_syncer(){};

Q_SIGNALS:
    void accepted();
    void rejected();
    
public Q_SLOTS:
    void accept(){ emit accepted(); }
    void reject(){ emit rejected(); }
};

#endif
