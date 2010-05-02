
#ifndef URT_CLIPPER_H
#define URT_CLIPPER_H

#include <QObject>
#include <QClipboard>

#include "pointers.h"

class clipper: public QObject {
    Q_OBJECT
public:
    clipper( QObject* parent, app_options_p opts);
    ~clipper();

Q_SIGNALS:
    void address_obtained( const QString& addr );
    
public Q_SLOTS:
    void changed( QClipboard::Mode mode );

private:
    app_options_p opts_;
    QString addr_;
};

#endif
