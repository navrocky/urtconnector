#ifndef WEAPON_FILTER_H
#define WEAPON_FILTER_H

#include <QWidget>

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>

class QButtonGroup;

class weapon_filter;

class weapon_filter_quick_opt_widget : public QWidget
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

template <bool Invert>
class weapon_filter_class : public filter_class
{
public:
    weapon_filter_class()
        : filter_class(get_id(),
            (Invert)
                ? QObject::tr("Forbidden weapon")
                : QObject::tr("Allowed weapon")
            , QObject::tr("Hides all servers thats don's match selected weapons."))
    {}

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent){
        return new weapon_filter_quick_opt_widget( f, parent );
    }
    
    virtual filter_p create_filter(){
        return filter_p( new weapon_filter( Invert, shared_from_this() ) );
    }

    const char* get_id(){
        return (Invert)
            ? "weapon_filter_f"
            : "weapon_filter_a";
    }
};

class weapon_filter : public filter
{
    Q_OBJECT
public:
    weapon_filter( bool invert, filter_class_p fc);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

    void insert( Gear g );
    void erase( Gear g );
    
    const std::set<Gear>& available() const;
    
private:
    bool invert_;
    std::set<Gear> available_;
};

#endif  /* WEAPON_FILTER_H */

