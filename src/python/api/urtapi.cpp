
#include <boost/function.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/python.hpp>

#include <QString>
#include <QtGui>

#include "common/server_id.h"
#include "common/server_bookmark.h"

#include "python/python_engine.h"


template <typename T>
struct smart_getter {};

template <typename T>
struct smart_getter<boost::shared_ptr<T> > {
    
    static T* get_ptr(boost::shared_ptr<T> ptr) {
        return ptr.get();
    }
};

/// super macros to make function call through boost::shared_ptr, when object stord in python by shared_ptr
#define MAKE_SMART_FUNC_0(type, func, ret)\
    make_function( \
        boost::bind(&type::func, boost::bind(smart_getter<boost::shared_ptr<type> >::get_ptr, _1)) \
        , default_call_policies() \
        , boost::mpl::vector<ret, boost::shared_ptr<type> >() \
    )
    
#define MAKE_SMART_FUNC_1(type, func, ret, arg1)\
    make_function( \
        boost::bind((ret (type::*)(arg1))(&type::func), boost::bind(smart_getter<boost::shared_ptr<type> >::get_ptr, _1), _2) \
        , default_call_policies() \
        , boost::mpl::vector<ret, boost::shared_ptr<type>, arg1>() \
    )
    
#define MAKE_SMART_FUNC_2(type, func, ret, arg1, arg2)\
    make_function( \
        boost::bind((ret (type::*)(arg1, arg2))(&type::func), boost::bind(smart_getter<boost::shared_ptr<type> >::get_ptr, _1), _2, _3) \
        , default_call_policies() \
        , boost::mpl::vector<ret, boost::shared_ptr<type>, arg1, arg2>() \
    )

namespace boost { namespace python { namespace objects {

template <typename Source, typename Target>
struct dynamic_cast_generator<boost::shared_ptr<Source>, Target>
{
    static void* execute(void* source)
    {
        return dynamic_cast<Target*>(
            static_cast<boost::shared_ptr<Source>*>(source)->get());
    }
        
};

}}};

long int unwrap(QObject* ptr) {
    return reinterpret_cast<long int>(ptr);
}


long int unwrap(boost::shared_ptr<QObject> ptr) {
    return reinterpret_cast<long int>(ptr.get());
}

template <typename T>
T* wrap(long int ptr) {
    return reinterpret_cast<T*>(ptr);
}



BOOST_PYTHON_MODULE(liburtapi)
{
    using namespace boost::python;
    
    class_<QObject, QObject*, boost::noncopyable>("QObject", no_init)
        .def("unwrap", (long int(*)(QObject*)) unwrap)
        .def("wrap", make_function( wrap<QObject>, return_value_policy<return_by_value>() ))
        .staticmethod("wrap");
        
    class_<QWidget, bases<QObject>, QWidget*, boost::noncopyable>("QWidget")
        .def("wrap", make_function( wrap<QWidget>, return_value_policy<return_by_value>() ))
        .staticmethod("wrap");
        
    class_<QFrame, bases<QWidget>, QFrame*, boost::noncopyable>("QFrame")
        .def("wrap", make_function( wrap<QFrame>, return_value_policy<return_by_value>() ))
        .staticmethod("wrap");
        
    class_<QMainWindow, bases<QWidget>, QMainWindow*, boost::noncopyable>("QMainWindow")
        .def("wrap", make_function( wrap<QMainWindow>, return_value_policy<return_by_value>() ))
        .staticmethod("wrap");
        
        
    class_<engine, bases<QObject>, engine*, boost::noncopyable>("engine", no_init)
        .def("add_tab", &engine::add_tab);
        
    
    class_<QString>("QString")
        .def(init<const char*>())
        .def("toStdString", &QString::toStdString);
    
    class_<server_id>("server_id")
        .def(init<const QString&, const QString&, int>())
        .def(init<const QString&, int>())
        .def(init<const QString&>())
        .add_property("ip", &server_id::ip, &server_id::set_ip)
        .add_property("hostname", &server_id::host_name, &server_id::set_host_name)
        .def("ip_or_host", &server_id::ip_or_host)
        .def("port", &server_id::port)
        .def("set_port", (void (server_id::*)(int))(&server_id::set_port))
        .def("set_port", (void (server_id::*)(const QString&))(&server_id::set_port))
        .def("address", &server_id::address)
        .def("empty", &server_id::is_empty);

    class_<server_bookmark>("server_bookmark")
        .def(init<const server_id&, const QString&, const QString&, const QString&, const QString&, const QString&>())
        .add_property("id",
            make_function(&server_bookmark::id, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_id))
        .add_property("name",
            make_function(&server_bookmark::name, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_name))
        .add_property("comment",
            make_function(&server_bookmark::comment, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_comment))
        .add_property("password",
            make_function(&server_bookmark::password, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_password))
        .add_property("rcon_password",
            make_function(&server_bookmark::rcon_password, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_rcon_password))
        .add_property("ref_password",
            make_function(&server_bookmark::ref_password, return_value_policy<copy_const_reference>()),
            make_function(&server_bookmark::set_ref_password))
        .def("empty", &server_bookmark::is_empty);
	
	
//     class_<server_bookmark_list_p, bases<QObject> >("server_bookmark_list")
//         .def("add", MAKE_SMART_FUNC_1(server_bookmark_list, add, void, const server_bookmark&) )
//         .def("change", MAKE_SMART_FUNC_1(server_bookmark_list, change, void, const server_bookmark&) )        
//         .def("change", MAKE_SMART_FUNC_2(server_bookmark_list, change, void, const server_id&, const server_bookmark&) )
//         .def("remove", MAKE_SMART_FUNC_1(server_bookmark_list, remove, void, const server_id&) )        
//         .def("remove", MAKE_SMART_FUNC_1(server_bookmark_list, remove, void, const server_bookmark&) )
//         .def("clear", MAKE_SMART_FUNC_0(server_bookmark_list, clear, void) )
// // //         .def("list", &server_bookmark_list::list);
//         .def("size", MAKE_SMART_FUNC_0(server_bookmark_list, size, int) );
        
        class_<server_bookmark_list, bases<QObject>, boost::noncopyable >("server_bookmark_list")
            .def("size", &server_bookmark_list::size);

        

        register_ptr_to_python< boost::shared_ptr<server_bookmark_list> >();
        
//         register_ptr_to_python< QWidget >();

    
}


