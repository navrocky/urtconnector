#include "base_exception.h"

namespace cl
{
namespace except
{

base_exception::base_exception(const std::string& message)
: runtime_error(message)
{
}

}
}