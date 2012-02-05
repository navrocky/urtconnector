#ifndef URT_PYTHON_CTX_H
#define URT_PYTHON_CTX_H

#include <boost/python.hpp>
#include <boost/python/call.hpp>

struct python_ctx {
	boost::python::object main_module;
	boost::python::object main_namespace;
};

#endif 
