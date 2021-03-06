#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

namespace anticheat
{

class sshot_output : public QObject
{
    Q_OBJECT
public:
    sshot_output(QObject* parent = NULL);

    bool is_enabled() const {return enabled_;}
    virtual void set_enabled(bool val);
    
    // prepare for sending
    virtual void start();

    // stop sending
    virtual void stop();

    virtual bool can_send_now() = 0;
    virtual void send_file(const QString& name, const QByteArray& data, bool heavy = false) = 0;

private:
    bool enabled_;
};

}