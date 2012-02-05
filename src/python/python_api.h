
#ifndef URT_PYTHON_API_H
#define URT_PYTHON_API_H

#include <boost/python.hpp>
#include <boost/python/call.hpp>

#include <QString>

struct python_ctx {
    
    python_ctx() : initialized(false){}
    
    boost::python::object main_module;
    boost::python::object main_namespace;
    bool initialized;
};


///Initialize Python interpretter and context
void python_init(python_ctx& ctx);

///Extract last python error from interpretter
std::string python_err(python_ctx& ctx);


///execute python expression
boost::python::object python_exec(const std::string& expression, python_ctx& ctx);

///execute python script in file \p fielename
boost::python::object python_exec_file(const std::string& filename, python_ctx& ctx);

///evaluate python expression and return it as variable with specified type
template <typename T>
T python_eval(const std::string& expression, python_ctx& ctx) {
    typedef boost::error_info<struct eval_error, std::string> eval_err;
    
	try {
		python_exec("result_tmp_transfer = " + expression, ctx);
		return boost::python::extract<T>(ctx.main_namespace["result_tmp_transfer"]);            	
	}
    catch(boost::python::error_already_set const &)
    {
        throw boost::enable_error_info(std::runtime_error(python_err(ctx))) << eval_err("Pyhton evaluation failed");
    }
    catch(boost::exception& e)
    {
        e << eval_err("Pyhton evaluation failed");
        throw;
    }   
}



#endif
