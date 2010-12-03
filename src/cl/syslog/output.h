#ifndef _LOG_OUTPUT_H
#define	_LOG_OUTPUT_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include <cl/syslog/filter.h>
#include <cl/syslog/message.h>

namespace cl
{
namespace syslog
{

class thread_info;

/*! Base class for log outputs */
class output
{
public:
    output();
    virtual ~output();

    /*! Add a filter to output. */
    void filter_add(filter_t flt);
    
    /*! Checks that a message is suitable to this output according to filters. */
    bool check(const message& msg);
    
    /*! Write a message to output. */
    void write(const message& msg, const thread_info& thread);
protected:

    /*! You need to redefine this method for specific out */
    virtual void do_write(const message& msg, const thread_info& thread) = 0;

private:
    typedef std::vector<filter_t> filter_list;
    filter_list filters_;
};

typedef boost::shared_ptr<output> output_p;

}
}

#endif	/* _LOG_OUTPUT_H */

