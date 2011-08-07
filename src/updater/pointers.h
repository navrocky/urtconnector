#ifndef POINTERS_H
#define	POINTERS_H

#include <boost/shared_ptr.hpp>
//#include <boost/weak_ptr.hpp>

class update_task;
typedef boost::shared_ptr<update_task> update_task_p;
//typedef boost::weak_ptr<update_task> update_task_wp;

#endif	/* POINTERS_H */

