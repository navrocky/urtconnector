#ifndef FILTER_H
#define	FILTER_H

#include <QString>
#include <QIcon>
#include <QObject>
#include <boost/enable_shared_from_this.hpp>

#include "pointers.h"

class server_info;

class filter : public QObject
{
    Q_OBJECT
public:
    filter(filter_class_p fc);

    /*! Unique filter identifier (name). */
    const QString& name() const {return name_;}
    void set_name(const QString& uid);

    /*! Get filter class. */
    filter_class_p get_class() const {return fc_;}

    bool enabled() const {return enabled_;}
    void set_enabled(bool val);

    /*! Filters server by server info. */
    virtual bool filter_server(const server_info& si) = 0;

    /*! Save filter settings to QByteArray. */
    virtual QByteArray save();

    /*! Load filter settings from QByteArray. */
    virtual void load(const QByteArray& ba, filter_factory_p factory);

signals:
    void changed_signal();

private:
    filter_class_p fc_;
    QString name_;
    bool enabled_;
};

/*! Filter class. Creation, info. */
class filter_class : public boost::enable_shared_from_this<filter_class>
{
public:
    filter_class(const QString& id, const QString& caption, const QString& description,
                 const QIcon& icon = QIcon());

    QString id() const {return id_;}
    QString caption() const {return caption_;}
    QString description() const {return description_;}
    const QIcon& icon() const {return icon_;}

    virtual filter_p create_filter() = 0;
    
    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual bool has_additional_options();
//    virtual prop_panel create_prop_panel() = 0;
private:
    QString id_;
    QString caption_;
    QString description_;
    QIcon icon_;
};

#endif	/* FILTER_H */

