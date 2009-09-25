#ifndef _QSTATOPTIONS_H
#define	_QSTATOPTIONS_H

#include <QString>

class QStatOptions
{
public:
    QStatOptions();

    // QStat binary path
    QString qstatPath;

    // URT master server ip or hostname
    QString masterServer;

};

#endif

