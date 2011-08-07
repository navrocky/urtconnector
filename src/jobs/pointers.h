#ifndef JOB_POINTERS_H
#define	JOB_POINTERS_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class job_t;
typedef boost::shared_ptr<job_t> job_p;
typedef boost::weak_ptr<job_t> job_weak_p;

#endif

