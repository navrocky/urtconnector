#ifndef FILTER_LIST_H
#define	FILTER_LIST_H

#include <QObject>
#include <QString>
#include <map>

#include "pointers.h"

class server_info;

class filter_list : public QObject
{
    Q_OBJECT
public:
    filter_list(filter_factory_p factory);

    /*! Filters factory. */
    filter_factory_p factory() const {return factory_;}

    /*! Root filter. */
    filter_p root_filter() const {return root_filter_;}
    void set_root_filter(filter_p f);

    filter_p create_by_class_id(const QString& id) const;

    /*! Filter selected for list toolbar. */
    filter_weak_p toolbar_filter() const {return toolbar_filter_;}

    /*! Change toolbar filter. */
    void set_toolbar_filter(filter_weak_p f);

    /*! Finds filter by name */
    filter_p find_by_name(const QString& name);

    /*! Checks that name of filter is correct, dont across with others. */
    bool is_name_correct(const QString& name);

    /*! Returns correctly filter name based on input name, that dont across
        with other filters. */
    QString correct_name(const QString& name);

    bool filtrate( const server_info& si) const;


signals:
    void toolbar_filter_changed();

private:
    filter_factory_p factory_;
    filter_p root_filter_;
    filter_weak_p toolbar_filter_;
};

#endif	/* FILTER_LIST_H */

