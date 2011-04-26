#ifndef TABS_MAIN_TAB_H
#define TABS_MAIN_TAB_H

#include <boost/shared_ptr.hpp>

#include <QMainWindow>

#include <common/server_id.h>
#include <filters/pointers.h>
#include <settings/settings.h>
#include "tab_context.h"

#include "../pointers.h"

class tab_settings;
typedef boost::shared_ptr<tab_settings> tab_settings_p;

///Class that provide base interface for any widget at main tab widget
class main_tab : public QMainWindow
{
    Q_OBJECT
public:
    main_tab(const tab_settings_p& st, const tab_context& ctx, QWidget* parent);

    /// returns currently selected server if any
    virtual server_id selected_server() const = 0;

signals:
    void contents_changed();
    void selection_changed();

public slots:
    virtual void save_state();
    virtual void load_state();

protected slots:
    /*! This function automatically called after server_list changed.
        Also emits contents_changed() signal. */
    virtual void server_list_changed();

protected:
    const tab_settings_p& settings() const { return st_; }
    server_list_p server_list() const { return ctx_.serv_list(); }
    const tab_context& context() const { return ctx_; }

private:
    tab_settings_p st_;
    tab_context ctx_;
};

class tab_settings
{
public:
    tab_settings(const QString& object_name);
    virtual ~tab_settings() {}

    void save_state(const QByteArray& a);
    QByteArray load_state() const;
    
    const QString& uid() const {return uid_;}

    //TODO backward config compatibility - remove on 0.8.0
    const base_settings::settings_ptr& ts() const { return st; }

private:
    QString uid_;
    base_settings::settings_ptr st;
};

#endif
