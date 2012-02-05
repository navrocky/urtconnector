
#ifndef URT_PYTHON_API_H
#define URT_PYTHON_API_H

#include <QString>
#include "python_ctx.h"

void python_init(python_ctx& ctx);

std::string python_err(python_ctx& ctx);

boost::python::object python_exec(const std::string& expression, python_ctx& ctx);


template <typename T>
T python_eval(const std::string& expression, python_ctx& ctx) {
	try {
		python_exec("result_tmp_transfer = " + expression, ctx);
		return boost::python::extract<T>(ctx.main_namespace["result_tmp_transfer"]);            	
	}
	catch(boost::python::error_already_set const &)
	{
		printf("Error in python_eval\r\n");
		PyErr_Print();
		throw std::runtime_error("Error in python_eval");
	}	
}





//! Run a python command
#define EXEC_PYTHON(command, context)                                                                                                      \
try                                                                                                                               \
{                                                                                                                                 \
    boost::python::object dummy_ignore_wasdy = boost::python::exec(std::string(command).c_str(), context.main_namespace, context.main_namespace); \
}                                                                                                                                 \
catch(boost::python::error_already_set const &)                                                                                          \
{                                                                                                                                 \
    printf("Error in Python execution of: \r\n%s\r\n", std::string(command).c_str());                                             \
    PyErr_Print();                                                                                                                \
    assert(0 && "Halting on Python error");                                                                                       \
    throw;                                                                                                                        \
}

//! Store the output of command in var_name, which has type output_type
#define EVAL_PYTHON(output_type, var_name, command, context)                                         \
output_type var_name;                                                                       \
try                                                                                         \
{                                                                                           \
    std::string full_command_wasdy = std::string("result_wasdy = ") + std::string(command); \
    EXEC_PYTHON(full_command_wasdy.c_str(), context);                                                \
    boost::python::object python_result_wasdy = context.main_namespace["result_wasdy"];             \
    var_name = boost::python::extract<output_type>(python_result_wasdy);                           \
}                                                                                           \
catch(boost::python::error_already_set const &)                                                    \
{                                                                                           \
    printf("Error in Python execution of: \r\n%s\r\n", std::string(command).c_str());       \
    PyErr_Print();                                                                          \
    assert(0 && "Halting on Python error");                                                 \
    throw;                                                                                  \
}

//! Get a value from python, of a particular type. Can be a variable name, or even a complete expression
template<class T>
T GET_PYTHON(std::string var_name, python_ctx& ctx)
{
    EVAL_PYTHON(T, result_wasdy2, var_name, ctx);
    return result_wasdy2;
};


//! Reflect a python object in a boost::python object. Used for security reasons to not enter parameters into strings and
//! falling prey to injection attacks

#define REFLECT_PYTHON(name, context) static boost::python::object name = GET_PYTHON<boost::python::object>(#name, context);

#endif
