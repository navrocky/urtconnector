#pragma once

#include "sshot_output.h"

namespace anticheat
{

class sshot_file_output : public sshot_output
{
    Q_OBJECT
public:
    sshot_file_output(QObject* parent = NULL);

    virtual bool can_send_now();
    virtual void send_file(const QString& name, const QByteArray& data, bool heavy = false);

    void set_folder(const QString& val);

private:
    QString folder_;
};

}
