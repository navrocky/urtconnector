#ifndef SETTINGS_GENERATOR_H
#define	SETTINGS_GENERATOR_H

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/shared_ptr.hpp>

/*
    Short example:

=================================
#include <settings_generator.h>

#define MY_SETTINGS \
        ((color, int, 10)) \
        ((name, double, 20.0))

SETTINGS_GENERATE_CLASS(my_settings, MY_SETTINGS)
=================================

 And don't forget to create class implementation. See at settings_generator_impl.h

*/

#define __GENERATE_METHODS_DECL(name, value_type, default_value) \
    value_type name() const; \
    void BOOST_PP_CAT(name, _set)(const value_type& val); \
    value_type BOOST_PP_CAT(name, _default)() const; \
    void BOOST_PP_CAT(name, _reset)();

#define __GENERATE_METHODS_DECL_MACRO(r, data, elem) __GENERATE_METHODS_DECL elem
#define __GENERATE_ALL_METHODS_DECL(opts_list) BOOST_PP_SEQ_FOR_EACH(__GENERATE_METHODS_DECL_MACRO,, opts_list)

#define SETTINGS_GENERATE_CLASS(class_name, __opts_list) \
class class_name \
{ \
public: \
    class_name(); \
    static const char* uid() {return BOOST_PP_STRINGIZE(class_name);}\
    __GENERATE_ALL_METHODS_DECL(__opts_list) \
private: \
    boost::shared_ptr<QSettings> s_; \
};

class QSettings;

#endif
