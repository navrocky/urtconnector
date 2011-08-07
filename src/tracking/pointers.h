#ifndef TRACKING_POINTERS_H
#define	TRACKING_POINTERS_H

#include <boost/shared_ptr.hpp>

namespace tracking
{

class context_t;
typedef boost::shared_ptr<context_t> context_p;

class task_t;
typedef boost::shared_ptr<task_t> task_p;

class condition_class;
typedef boost::shared_ptr<condition_class> condition_class_p;

class condition_t;
typedef boost::shared_ptr<condition_t> condition_p;

class condition_factory;
typedef boost::shared_ptr<condition_factory> condition_factory_p;

class action_class;
typedef boost::shared_ptr<action_class> action_class_p;

class action_t;
typedef boost::shared_ptr<action_t> action_p;

class action_factory;
typedef boost::shared_ptr<action_factory> action_factory_p;

}

#endif

