#ifndef TOOLS_H
#define	TOOLS_H

#include <QByteArray>
#include "pointers.h"

/*! Save filter to QByteArray. */
QByteArray filter_save(filter_p f);

/*! Load filter from QByteArray. */
filter_p filter_load(const QByteArray& ba, filter_factory_p factory);

#endif	/* TOOLS_H */

