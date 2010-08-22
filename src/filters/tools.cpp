#include <QDataStream>
#include <cl/except/error.h>

#include "filter.h"
#include "filter_factory.h"
#include "tools.h"

QByteArray filter_save(filter_p f)
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; //version
    if (f)
    {
        ds << f->get_class()->id();
        ds << f->enabled();
        ds << f->save();
    } else
        ds << QString(); // empty class name - no filter

    return res;
}

filter_p filter_load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);
    
    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");
    
    // create filter
    QString id;
    ds >> id;

    // no filter?
    if (id.isEmpty())
        return filter_p();

    filter_p res = factory->create_filter_by_id(id);

    bool enabled;
    ds >> enabled;
    res->set_enabled(enabled);
    
    QByteArray ba2;
    ds >> ba2;
    res->load(ba2, factory);
    
    return res;
}

