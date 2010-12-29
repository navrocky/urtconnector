
#ifndef URT_MAIN_TAB_H
#define URT_MAIN_TAB_H

#include <QMainWindow>

#include <common/server_id.h>
#include <filters/pointers.h>
#include <settings/settings.h>

#include "pointers.h"


///Class that provide base interface for any widget at main tab widget
class main_tab : public QMainWindow {
    Q_OBJECT
public:

    explicit main_tab( const QString& object_name, QWidget* parent = 0, filter_factory_p factory = filter_factory_p() );
    virtual ~main_tab();
    
    ///returns currently selected server if any
    virtual server_id selected_server() const;

    /*! Current selection in tab */
    virtual server_id_list selection() const;
    
    void set_server_list(server_list_p ptr);
    server_list_p server_list() const;

    

    const filter_list& filterlist() const;
signals:
    void contents_changed();

public Q_SLOTS:
    void update_servers();
    void force_update_servers();
    
protected Q_SLOTS:

    ///this function automatically called when server_list has some changes
    virtual void servers_updated();
    virtual void filter_changed();

private Q_SLOTS:
    
    void update_toolbar_filter();
    void show_filter(bool);
    void save_filter();

protected:
    void init_filter_toolbar();
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;

};

class tab_settings
{
public:
    tab_settings(const QString& object_name);
    
    filter_p root_filter(filter_factory_p factory) const;
    void save_root_filter(filter_p f);

    QString toolbar_filter() const;
    void save_toolbar_filter(const QString&);

    void save_state(const QByteArray& a);
    QByteArray load_state() const;

    bool is_filter_visible() const;
    void set_filter_visible(bool val);
private:
    base_settings::settings_ptr st;
};

#endif

