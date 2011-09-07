#ifndef SETTINGS_GENERATOR_IMPL_H
#define	SETTINGS_GENERATOR_IMPL_H

/*
    This macro must be used in .cpp. Short example:

=================================
#include "my_settings.h"
#include <settings_generator_impl.h>

SETTINGS_GENERATE_CLASS_IMPL(my_settings, MY_SETTINGS)
=================================
*/

#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <QVariant>
#include <QSettings>
#include "settings.h"

#define __GENERATE_METHODS_IMPL(class_name, name, value_type, default_value) \
    value_type class_name::name() const \
    { \
        return s_->value(BOOST_PP_STRINGIZE(name), QVariant::fromValue(default_value)).value<value_type>(); \
    } \
    void class_name::BOOST_PP_CAT(name, _set)(const value_type& val) \
    { \
        s_->setValue(BOOST_PP_STRINGIZE(name), QVariant::fromValue(val)); \
    } \
    value_type class_name::BOOST_PP_CAT(name, _default)() const \
    { \
        return value_type(default_value); \
    } \
    void class_name::BOOST_PP_CAT(name, _reset)() \
    { \
        s_->remove(BOOST_PP_STRINGIZE(name)); \
    }

#define __PARAM_1(a, b, c) a
#define __PARAM_2(a, b, c) b
#define __PARAM_3(a, b, c) c
#define __GENERATE_METHODS_IMPL_MACRO(r, data, elem) __GENERATE_METHODS_IMPL(data, __PARAM_1 elem, __PARAM_2 elem, __PARAM_3 elem)
#define __GENERATE_ALL_METHODS_IMPL(class_name, opts_list) BOOST_PP_SEQ_FOR_EACH(__GENERATE_METHODS_IMPL_MACRO, class_name, opts_list)

#define SETTINGS_GENERATE_CLASS_IMPL(class_name, opts_list) \
    class_name::class_name() : s_( base_settings::get_settings(BOOST_PP_STRINGIZE(class_name))){} \
    __GENERATE_ALL_METHODS_IMPL(class_name, opts_list)
#endif
