#ifndef CLASS_LIST_H
#define CLASS_LIST_H

#include <cassert>
#include <map>
#include <boost/typeof/typeof.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/call_traits.hpp>

/*! Standard class traits. Classes stored by value.
    Operator () used for create objects. */
template <class ClassType>
struct class_traits
{
    typedef BOOST_TYPEOF_TPL(&ClassType::operator()) func_t;
    typedef typename boost::result_of<func_t()>::type result_type;
    typedef typename boost::call_traits<ClassType>::param_type class_param;

    static result_type create(class_param f)
    {
        return f();
    }
};

/*! Abstract class_list. Store factories, provides access to them by key.

Example:
\code
// If a class conform to followed requirements (stored by value in list and
// create objects with an operator () without parameters) then a declaration
// can be simple.

typedef class_list<my_base_class> my_class_list_t;

my_class_list_t fl;
fl.add_class("first", my_first_class());
fl.add_class("second", my_second_class());

my_object obj = fl.create("first");

// If a class not conform to requirements above, then you have to define an own
// specialization of a class_traits as shown below.

template <>
struct class_traits<property_class_p>
{
    typedef property_p result_type;
    static result_type create(property_class_p f)
    {
        return f->create();
    }
};
typedef class_list<property_class_p> property_class_list_t;

\endcode
 */
template <class ClassType, class NameType = std::string>
class class_list
{
private:
    typedef NameType name_t;
    typedef ClassType class_t;
    typedef typename boost::call_traits<name_t>::param_type name_param;
    typedef typename boost::call_traits<class_t>::param_type class_param;
    typedef typename boost::call_traits<class_t>::const_reference class_const_ref;

public:
    typedef std::map<name_t, class_t> classes_t;
    /*! class result type. */
    typedef typename class_traits<class_t>::result_type result_type;
    typedef ClassType class_type;

    /*! Add class to list with name. */
    void add_class(name_param name, class_param f)
    {
        assert(!is_exists(name));
        classes_[name] = f;
    }

    /*! Remove class from list by name. */
    void remove_class(name_param name)
    {
        typename classes_t::iterator it = classes_.find(name);
        assert(it != classes_.end());
        classes_.erase(it);
    }

    class_const_ref get(name_param name) const
    {
        typename classes_t::const_iterator it = classes_.find(name);
        assert(it != classes_.end());
        return it->second;
    }

    /*! Checks that name is registererd on list. */
    bool is_exists(name_param name) const
    {
        typename classes_t::const_iterator it = classes_.find(name);
        return it != classes_.end();
    }

    /*! class list */
    const classes_t& classes() const {return classes_;}

    /*! Create object without parameters. */
    result_type create(name_param name) const
    {
        return class_traits<class_t>::create(get(name));
    }

    /*! Create object with the one parameter. Specialization of a class_traits
        must contain create method with the one additional parameter. */
    template <typename T1>
    result_type create(name_param name, T1 t1) const
    {
        return class_traits<class_t>::create(get(name), t1);
    }

    /*! Create object with a two parameters. Specialization of a class_traits
        must contain create method with a two additional parameters. */
    template <typename T1, typename T2>
    result_type create(name_param name, T1 t1, T2 t2) const
    {
        return class_traits<class_t>::create(get(name), t1, t2);
    }

private:
    classes_t classes_;
};

#endif
