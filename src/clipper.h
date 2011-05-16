#ifndef URT_CLIPPER_H
#define URT_CLIPPER_H

#include <QObject>
#include <QClipboard>

#include "pointers.h"

class clipper: public QObject
{
    Q_OBJECT
public:
    clipper ( QObject* parent );
    ~clipper();
    
    QString address() const {return address_;}
    QString password() const {return password_;}

signals:
    void info_obtained();

public slots:
    void changed ( QClipboard::Mode mode );

private:
    QString address_;
    QString password_;
};

#endif
