#include "sshot_output.h"

sshot_output::sshot_output(QObject* parent)
: QObject(parent)
, enabled_(true)
{
}

void sshot_output::set_enabled(bool val)
{
    enabled_ = val;
}

void sshot_output::start()
{
}

void sshot_output::stop()
{
}

