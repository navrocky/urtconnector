
#ifndef URT_RCON_SETTINGS_FORM_H
#define URT_RCON_SETTINGS_FORM_H

#include <memory>
#include <map>

#include <QWidget>

class rcon_settings_form: public QWidget {
Q_OBJECT
public:
    rcon_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~rcon_settings_form();

private Q_SLOTS:
    void custom_checked( bool b );
    void color_clicked();
    void reset_to_defaults();
private:
    void init();    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;

};


#endif
