
#include <boost/function.hpp>

#include <boost/python/to_python_converter.hpp>

#include <boost/mpl/vector.hpp>

#include <QString>

#include "urt_api.h"

char const* greet()
{
	std::cerr<<"This is fucken greet!"<<std::endl;
   return "hello, world";
}


struct QString_to_python_str
{
    static PyObject* convert(const QString& s)
    {
        return boost::python::incref(
            boost::python::object(
                s.toUtf8().constData()).ptr());
    }
    
    
};

// register the QString-to-python converter


std::string to_str(server_bookmark* bm)
{
	return bm->name().toStdString();
}


template <typename T>
struct smart_getter {};

template <typename T>
struct smart_getter<boost::shared_ptr<T> > {
    
    static T* get_ptr(boost::shared_ptr<T> ptr) {
        return ptr.get();
    }
};


template <typename B, typename Func>
bool test_f(B b) {
//     return (b->*Func)();
};

struct tester {
    bool yep()
    {
        return true;
    }
};
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


BOOST_PYTHON_MODULE(liburt_api)
{
//     test_m<&server_bookmark_list::size> t;
    
    using namespace boost::python;
    
    tester t;
    
//     test_f<tester*, &tester::>(&t);
    
//     boost::python::to_python_converter<QString, QString_to_python_str>();
    
    def("greet", greet);
    
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
	
	
//     class_<QObject*>("QObject");
        
        struct GS {
            static void add(server_bookmark_list_p l, const server_bookmark& bm ){
                l->add(bm);
            }
        };
        
        boost::function<void(server_bookmark_list_p, server_bookmark)> f =
        boost::bind(&server_bookmark_list::add
            , boost::bind(smart_getter<server_bookmark_list_p>::get_ptr, _1)
            , _2);
        
    class_<server_bookmark_list_p>("server_bookmark_list")
        .def("add", MAKE_SMART_FUNC_1(server_bookmark_list, add, void, const server_bookmark&) )
        .def("change", MAKE_SMART_FUNC_1(server_bookmark_list, change, void, const server_bookmark&) )        
        .def("change", MAKE_SMART_FUNC_2(server_bookmark_list, change, void, const server_id&, const server_bookmark&) )
        .def("remove", MAKE_SMART_FUNC_1(server_bookmark_list, remove, void, const server_id&) )        
        .def("remove", MAKE_SMART_FUNC_1(server_bookmark_list, remove, void, const server_bookmark&) )
        .def("clear", MAKE_SMART_FUNC_0(server_bookmark_list, clear, void) )
        
    
//             .def("add", make_function( 
//                             f
//                             , default_call_policies()
//                             , boost::mpl::vector<void, server_bookmark_list_p, server_bookmark>()
//                         ))
//         .def("change", (void (server_bookmark_list::*)(const server_id&, const server_bookmark&))(&server_bookmark_list::change))
//         .def("change", (void (server_bookmark_list::*)( const server_bookmark&))(&server_bookmark_list::change))
//         .def("remove", (void (server_bookmark_list::*)(const server_id&))(&server_bookmark_list::remove))
//         .def("remove", (void (server_bookmark_list::*)(const server_bookmark&))(&server_bookmark_list::remove))
//         .def("clear", &server_bookmark_list::clear)
//         .def("get", &server_bookmark_list::get, return_value_policy<copy_const_reference>())
// //         .def("list", &server_bookmark_list::list);
        .def("size", MAKE_SMART_FUNC_0(server_bookmark_list, size, int) );

            
        
        
        
      
	

    
}


