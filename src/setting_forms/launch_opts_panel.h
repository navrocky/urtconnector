#ifndef LAUNCH_OPTS_PANEL_H
#define LAUNCH_OPTS_PANEL_H

#include <QWidget>

namespace Ui
{
class launch_opts_panel;
}

class launch_opts_panel : public QWidget
{
    Q_OBJECT
    
public:
    explicit launch_opts_panel(QWidget *parent = 0);
    ~launch_opts_panel();

    QString binary_path() const;
    void set_binary_path(const QString&);

    QString adv_cmd_line() const;
    void set_adv_cmd_line(const QString&);

    bool use_adv_cmd_line() const;
    void set_use_adv_cmd_line(bool);

    void set_separate_xsession(bool);
    void set_use_mumble_overlay(bool);
    void set_mumble_overlay_bin(const QString&);

signals:
    void changed();

private slots:
    void choose_binary();
    void insert_file_path();
    void update_launch_string();
    void int_changed();
    
private:
    Ui::launch_opts_panel *ui;
    bool lock_change_;
    bool separate_xsession;
    bool use_mumble_overlay;
    QString mumble_overlay_bin;
};

#endif // LAUNCH_OPTS_PANEL_H
