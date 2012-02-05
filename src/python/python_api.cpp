
#include <boost/throw_exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QApplication>

#include <cl/syslog/syslog.h>
#include "python_api.h"

SYSLOG_MODULE(pyhton_api)

namespace py = boost::python;

typedef boost::error_info<struct initialize_error, std::string> init_err;
typedef boost::error_info<struct exec_error, std::string> exec_err;
typedef boost::error_info<struct exec_file_error, std::string> exec_file_err;

class stderr_catcher {
public:
    void write(const std::string & message) {
        msg += message;
    }
    
    std::string msg;
};

std::string python_err(python_ctx& ctx)
{
    try {
        py::object stderr_orig = ctx.main_namespace["sys"].attr("stderr");
        ctx.main_namespace["sys"].attr("stderr") = stderr_catcher();
	
        PyErr_Print();	
        stderr_catcher catcher = py::extract<stderr_catcher>(ctx.main_namespace["sys"].attr("stderr"));
        ctx.main_namespace["sys"].attr("stderr") = stderr_orig;
        return  catcher.msg;
    }
    catch(...)
    {
        PyErr_Print();      
        return "Can't extract python error: something goes very wrong!";
    }
}


void python_init(python_ctx& ctx)
{
    try
    {
        Py_Initialize();

        py::class_<stderr_catcher>("stderr_catcher")
            .def("write", &stderr_catcher::write);
        
	
        //Я не знаю что ИМЕННО это значит
        ctx.main_module = py::import("__main__");
        ctx.main_namespace = ctx.main_module.attr("__dict__");

        //и не знаю почему надо писать так но задача следующая -
        // загркзить "sys" чтоб он было доступен как "sys"
        ctx.main_namespace["sys"] = py::import("sys");

        LOG_DEBUG << "configuring pyhton environment";
        
        python_exec(
            "sys.path.append('./')\n"
            "import liburtapi\n",
            ctx
        );

        //FIXME //HACK :)
        QString py_main = QFileInfo(QSettings(qApp->applicationName(), qApp->applicationName()).fileName()).dir().path();
        py_main += "/scripts/main.py";
        
        LOG_DEBUG << "loading main.py...";
        boost::python::exec_file(py_main.toStdString().c_str(), ctx.main_namespace, ctx.main_namespace);
        ctx.initialized = true;
    }
    catch(py::error_already_set const &)
    {
        throw boost::enable_error_info(std::runtime_error(python_err(ctx))) << init_err("Pyhton initialization failed");
    }
    catch(boost::exception& e)
    {
        e << init_err("Pyhton initialization failed");
        throw;
    }    
    LOG_DEBUG << "python initialized";
}

boost::python::object python_exec(const std::string& expression, python_ctx& ctx)
{
    try
    {	
        LOG_HARD << "python_exec: %1", expression;
        return boost::python::exec(expression.c_str(), ctx.main_namespace, ctx.main_namespace);
    }
    catch(py::error_already_set const &)
    {
        throw boost::enable_error_info(std::runtime_error("Executing of python expression failed")) << exec_err(python_err(ctx));
    }	
}

boost::python::api::object python_exec_file(const std::string& filename, python_ctx& ctx)
{
    try
    {   
        LOG_HARD << "python_exec_file: %1", filename;
        return boost::python::exec_file(filename.c_str(), ctx.main_namespace, ctx.main_namespace);
    }
    catch(py::error_already_set const &)
    {
        throw boost::enable_error_info(std::runtime_error("Executing of python file failed")) << exec_err(python_err(ctx));
    }   
}



