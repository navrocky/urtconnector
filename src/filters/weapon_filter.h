#ifndef WEAPON_FILTER_H
#define WEAPON_FILTER_H

#include <QGroupBox>

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>

class QButtonGroup;

class weapon_filter;

class weapon_filter_quick_opt_widget : public QGroupBox
{
    Q_OBJECT
public:
    weapon_filter_quick_opt_widget(filter_p f, QWidget* parent);

private Q_SLOTS:
    void clicked( int gear );
    void filter_changed();
    
   
private:
    QButtonGroup*   group_;
    weapon_filter&  filter_;
};

class weapon_filter_class : public filter_class
{
public:
    weapon_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};

class weapon_filter : public filter
{
    Q_OBJECT
public:
    weapon_filter(filter_class_p fc);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

    void insert( Gear g );
    void erase( Gear g );
    
    const std::set<Gear>& available() const;
    
private:
    friend class weapon_filter_quick_opt_widget;
    std::set<Gear> available_;
};

#endif  /* GAME_TYPE_FILTER_H */

