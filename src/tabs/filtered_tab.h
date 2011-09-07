#ifndef FILTERED_TAB_H
#define	FILTERED_TAB_H

#include <QPointer>

#include <filters/pointers.h>

#include "main_tab.h"

class QAction;
class QDockWidget;


class filtered_tab_settings
{
public:
    filtered_tab_settings(const tab_settings_p& ts);

    const QString& uid();
    
    filter_p root_filter(filter_factory_p factory) const;
    void save_root_filter(filter_p f);

    QString toolbar_filter() const;
    void save_toolbar_filter(const QString&);   
    
    bool is_filter_visible() const;
    void set_filter_visible(bool val);
    
    void save_state(const QByteArray& a);
    QByteArray load_state() const;

private:
    QString uid_;
    base_settings::qsettings_p fs;
    
    //TODO backward config compatibility - remove on 0.8.0
    base_settings::qsettings_p ts_; // - the main_tab settings
};

class filtered_tab : public main_tab
{
    Q_OBJECT
public:
    filtered_tab(tab_settings_p st,
                 const tab_context& ctx,
                 bool complex_filter,
                 QWidget* parent);

public slots:
    virtual void save_state();
    virtual void load_state();

protected slots:
    virtual void filter_changed();

protected:
    void default_filter_initialization();
    bool filtrate(const server_info& si) const;
    const filter_list_p& filters() const {return filters_;}
    void load_filter();

private slots:
    void save_filter();
    void update_toolbar_filter();

private:

    filter_list_p filters_;
    QPointer<QDockWidget> filter_widget_;
    QWidget* filter_holder_;
    QPointer<QWidget> filter_toolbar_widget_;
    QAction* show_filter_action_;
    filtered_tab_settings fs_;
    bool complex_filter_;
};


#endif