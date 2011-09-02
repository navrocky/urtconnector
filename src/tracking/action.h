#ifndef TRACKING_ACTION_H
#define	TRACKING_ACTION_H

#include <boost/enable_shared_from_this.hpp>

#include <QString>
#include <QIcon>
#include <QObject>

#include <common/class_list.h>

#include "pointers.h"
#include "common.h"

namespace tracking
{

class action_class : public boost::enable_shared_from_this<action_class>
{
public:
    action_class(const context_p& ctx,
                 const QString& id,
                 const QString& caption,
                 const QString& description,
                 const QIcon& icon = QIcon());

    const context_p& context() const {return ctx_;}

    QString id() const {return id_;}
    QString caption() const {return caption_;}
    QString description() const {return description_;}
    const QIcon& icon() const {return icon_;}

    virtual action_p create() = 0;

private:
    context_p ctx_;
    QString id_;
    QString caption_;
    QString description_;
    QIcon icon_;
};

class action_t : public QObject
{
    Q_OBJECT
public:
    enum result_t
    {
        r_continue, // continue actions execution
        r_cancel, // cancels actions execution
        r_skip // skip condition trigger and try again
    };

    action_t(const action_class_p& c);

    const action_class_p& get_class() const {return class_;}

    virtual result_t execute() = 0;

    virtual void save(settings_t& s) {}
    virtual void load(const settings_t& s) {}
    virtual void assign(action_t* src);

    virtual QWidget* create_options_widget(QWidget* parent) = 0;

signals:
    void changed();

private:
    action_class_p class_;
};

}

// define factory
template <>
struct class_traits<tracking::action_class_p>
{
    typedef tracking::action_p result_type;
    static result_type create(tracking::action_class_p f)
    {
        return f->create();
    }
};

namespace tracking
{
class action_factory : public class_list<action_class_p, QString>
{
public:
    action_factory(const context_p& ctx)
    : ctx_(ctx)
    {}

    template <typename T>
    void reg()
    {
        boost::shared_ptr<T> cl(new T(ctx_));
        add_class(cl->id(), cl);
    }
private:
    context_p ctx_;
};}

#endif	/* ACTION_H */

