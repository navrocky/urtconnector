#include <QSettings>

#include "state_settings.h"

QByteArray state_settings::geometry() const
{
    return part()->value("geometry").toByteArray();
}

void state_settings::set_geometry(const QByteArray& g)
{
    return part()->setValue("geometry", g );
}
