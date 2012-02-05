
// char const* greet()
// {
//    return "hello, world";
// }




// BOOST_PYTHON_MODULE(hello_ext)
// {
//     using namespace boost::python;
//     def("greet", greet);
// }


#include "python_api.h"
#include <QSettings>
#include <QFileInfo>
#include <QDir>

namespace py = boost::python;


class stderr_catcher {
public:
    void write(const std::string & message) {
	if (!msg.empty()) msg += "\n";
	msg += message;
    }
    
    std::string msg;
};

std::string python_err(python_ctx& ctx)
{
	py::object stderr_orig = ctx.main_namespace["sys"].attr("stderr");
	ctx.main_namespace["sys"].attr("stderr") = stderr_catcher();
	
        PyErr_Print();	
	stderr_catcher catcher = py::extract<stderr_catcher>(ctx.main_namespace["sys"].attr("stderr"));
	ctx.main_namespace["sys"].attr("stderr") = stderr_orig;
	return  catcher.msg;
}


void python_init(python_ctx& ctx)
{
    try
    {
        Py_Initialize();

	
        ctx.main_module = py::import("__main__");
        ctx.main_namespace = ctx.main_module.attr("__dict__");

        ctx.main_namespace["sys"] = py::import("sys");

        python_exec(
            "sys.path.append('./')\n"
            "import liburt_api\n",
            ctx
        );

        py::class_<stderr_catcher>("stderr_catcher")
            .def("write", &stderr_catcher::write);
	
//         QString py_main = QFileInfo(QSettings().fileName()).dir().path();
        QString py_main = QSettings().fileName();
        py_main += "/main.py";
        
        boost::python::exec_file(py_main.toStdString().c_str(), ctx.main_namespace);

// 	python_exec("a = b", ctx);
	
// 	ctx.main_namespace["liburt_api"] = py::import("liburt_api");
	  // Register the module with the interpreter


	
    }
    catch(py::error_already_set const &)
    {
        throw std::runtime_error(python_err(ctx));
    }		
}

boost::python::object python_exec(const std::string& expression, python_ctx& ctx)
{
    try
    {	
        return boost::python::exec(expression.c_str(), ctx.main_namespace);
    }
    catch(py::error_already_set const &)
    {
        throw std::runtime_error(python_err(ctx));
    }	
}


// int main(int argc, char **argv)
// {
//     
//     
// 
// 
//     python::object ignored = python::exec("hello = file('hello.txt', 'w')\n"
//                       "hello.write('Hello world!')\n"
//                       "hello.close()",
//                       python_main_namespace);
    
    // Set up general stuff to allow Python to be used
/*    EXEC_PYTHON("import sys, os");*/


    // Pass C commandline arguments untouched to Python
 /*   try
    {
        boost::python::list args;
        for (int i = 0; i < argc; i++)
            args.append(std::string(argv[i]));
        REFLECT_PYTHON( set_python_args )
        set_python_args(args);
    } catch(boost::python::error_already_set const &)
    {
        printf("Error in Python execution of setting args\r\n");
        PyErr_Print();
        assert(0 && "Halting on Python error");
    }
   */ 
    
// }



