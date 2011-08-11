#ifndef TRACKING_TASK_H
#define	TRACKING_TASK_H

#include <QObject>
#include <QList>
#include <QString>

#include "common.h"
#include "pointers.h"

namespace tracking
{

typedef QList<action_p> actions_t;

class task_t : public QObject
{
    Q_OBJECT
public:
    enum operation_mode_t
    {
        om_multi_trigger,
        om_single_trigger,
        om_destroy_after_trigger
    };

    task_t(QObject* parent = 0);

    const QString& id() const {return id_;}
    void set_id(const QString&);

    operation_mode_t operation_mode() const {return op_mode_;}
    void set_operation_mode(operation_mode_t op);

    const QString& caption() const {return caption_;}
    void set_caption(const QString& caption) {caption_ = caption;}
    
    const condition_p& condition() const {return cond_;}
    void set_condition(const condition_p& cond);


    void add_action(const action_p&);
    void remove_action(const action_p&);
    int move_action(const action_p&, int delta);

    const actions_t& actions() {return actions_;}

    void assign(task_t* src);
    void save(settings_t& s);
    void load(const settings_t& s, const condition_factory_p& conds,
        const action_factory_p& acts);

signals:
    void changed();

private slots:
    void condition_triggered();

private:
    void do_changed();

    QString id_;
    QString caption_;
    condition_p cond_;
    actions_t actions_;
    operation_mode_t op_mode_;
    bool block_changed_;
    bool block_execute_;
};

}

#endif

