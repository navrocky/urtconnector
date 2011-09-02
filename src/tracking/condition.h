#ifndef TRACKING_CONDITION_H
#define	TRACKING_CONDITION_H

#include <boost/enable_shared_from_this.hpp>

#include <QObject>
#include <QString>
#include <QIcon>

#include <common/class_list.h>

#include "pointers.h"
#include "common.h"

class QWidget;

namespace tracking
{

class condition_class : public boost::enable_shared_from_this<condition_class>
{
public:
    condition_class(const context_p& ctx,
                    const QString& id,
                    const QString& caption,
                    const QString& description,
                    const QIcon& icon = QIcon());

    const context_p& context() const {return ctx_;}

    QString id() const {return id_;}
    QString caption() const {return caption_;}
    QString description() const {return description_;}
    const QIcon& icon() const {return icon_;}

    virtual condition_p create() = 0;

private:
    context_p ctx_;
    QString id_;
    QString caption_;
    QString description_;
    QIcon icon_;
};

class condition_t : public QObject
{
    Q_OBJECT
public:
    condition_t(const condition_class_p& c);

    const condition_class_p& get_class() const {return class_;}

    void start();
    void stop();
    void restart();
    bool is_started() const {return started_;}

    // condition must be started after load
    bool is_start_needed() const {return is_start_needed_;}
    
    virtual void save(settings_t& s);
    virtual void load(const settings_t& s);

    // default implementation using save-load methods
    virtual void assign(condition_t* src);

    virtual QWidget* create_options_widget(QWidget* parent) = 0;

    virtual void skip_current() = 0;

signals:
    void triggered();
    void changed();

protected slots:
    // when condition became suitable call this method
    void trigger();

protected:
    virtual void do_start() = 0;
    virtual void do_stop() = 0;

private:
    condition_class_p class_;
    bool started_;
    bool is_start_needed_;
};

}

// define factory
template <>
struct class_traits<tracking::condition_class_p>
{
    typedef tracking::condition_p result_type;
    static result_type create(tracking::condition_class_p f)
    {
        return f->create();
    }
};

namespace tracking
{
class condition_factory : public class_list<condition_class_p, QString>
{
public:
    condition_factory(const context_p& ctx)
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
};
}

#endif
