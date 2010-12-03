#pragma once

#include <QString>

#include "anticheat.h"

namespace anticheat
{

/*! Create anticheat, initialized from setting */
anticheat* create_anticheat(const QString& player_name, QObject* parent = NULL);

}
