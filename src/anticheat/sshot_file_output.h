#pragma once

#include "sshot_output.h"

class sshot_file_output : public sshot_output
{
    Q_OBJECT
public:
    sshot_file_output(QObject* parent = NULL);

    virtual bool can_send_now();
    virtual void send_file(const QString& name, const QByteArray& data);
};

