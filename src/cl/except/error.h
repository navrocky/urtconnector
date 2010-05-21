#ifndef _CL_EXCEPT_ERROR_H
#define	_CL_EXCEPT_ERROR_H

#include <cl/except/base_exception.h>

namespace cl
{
namespace except
{

/*! \brief A base class for errors. */
class error: public base_exception
{
public:
    error(const std::string& msg);
};

}
}

#endif	/* _CL_EXCEPT_ERROR_H */

