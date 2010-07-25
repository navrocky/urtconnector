#ifndef POINTERS_H
#define	POINTERS_H

#include <vector>
#include <boost/shared_ptr.hpp>

class filter;
typedef boost::shared_ptr<filter> filter_p;

typedef std::vector<filter_p> filters_t;

class filter_class;
typedef boost::shared_ptr<filter_class> filter_class_p;

class filter_factory;
typedef boost::shared_ptr<filter_factory> filter_factory_p;

class filter_list;
typedef boost::shared_ptr<filter_list> filter_list_p;

#endif	/* POINTERS_H */

