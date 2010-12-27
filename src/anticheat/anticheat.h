#pragma once

#include <QObject>
#include <QList>
#include <QPointer>

#include <common/server_id.h>

class QEvent;

namespace anticheat
{

class sshot_output;

/*! Anticheat sends a screenshots with md5 to the public ftp server */

class anticheat : public QObject
{
    Q_OBJECT
public:
    anticheat(QObject* parent = NULL);
    ~anticheat();
    
    // interval between screenshots in milliseconds
    int interval() const {return interval_;}
    void set_interval(int val);

    int quality() const {return quality_;}
    void set_quality(int val);

    const QString& nick_name() const {return nick_name_;}
    void set_nick_name(const QString& val);

    bool is_started();

    virtual bool event(QEvent* e);

    void add_output(sshot_output* output);

public slots:
    void start();
    void stop();

private:
    void update_timer();
    void screen_shot();
    
    QList<QPointer<sshot_output> > outputs_;
    int interval_;
    int quality_;
    int timer_;
    QString nick_name_;
};

}
