#ifndef URT_CLIPPER_H
#define URT_CLIPPER_H

#include <QObject>
#include <QClipboard>

#include "pointers.h"

class clipper: public QObject
{
    Q_OBJECT
public:
    clipper ( QObject* parent, app_options_p opts );
    ~clipper();
    
    QString address() const {return address_;}
    QString password() const {return password_;}

signals:
    void info_obtained();

public slots:
    void changed ( QClipboard::Mode mode );

private:
    app_options_p opts_;
    QString address_;
    QString password_;
};

#endif
