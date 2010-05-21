#ifndef _CL_EXCEPT_BASE_EXCEPTION_H
#define	_CL_EXCEPT_BASE_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace cl
{
namespace except
{

/*! \brief Common library exception class. */
class base_exception : public std::runtime_error
{
public:
    base_exception(const std::string& message);
};

}
}

#endif	/* _CL_EXCEPT_BASE_EXCEPTION_H */

