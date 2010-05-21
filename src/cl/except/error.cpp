#include "error.h"

using namespace cl::except;

error::error(const std::string& msg)
: base_exception(msg)
{
}

