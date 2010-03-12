#ifndef _QSTATOPTIONS_H
#define	_QSTATOPTIONS_H

#include <QString>

class qstat_options
{
public:
    qstat_options();

    QString qstat_path;
    QString master_server;

};

#endif	/* _QSTATOPTIONS_H */

