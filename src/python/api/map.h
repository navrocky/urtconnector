
#ifndef URT_PYTHON_API_MAP_H
#define URT_PYTHON_API_MAP_H

#include <map>
#include <list>

#include <boost/python/errors.hpp>
#include <boost/tr1/tuple.hpp>

template<typename Map>
struct map_item
{
    typedef typename Map::key_type key_type;
    typedef typename Map::mapped_type mapped_type;

    static mapped_type& get(Map& self, const key_type idx) {
      if( self.find(idx) != self.end() ) return self.at(idx);
      PyErr_SetString(PyExc_KeyError,"Map key not found");
      boost::python::throw_error_already_set();
    }


    static void set(Map& self, const key_type idx, const mapped_type val) { self[idx]=val; }

    static void del(Map& self, const key_type n) { self.erase(n); }

    static bool in(Map const& self, const key_type n) { return self.find(n) != self.end(); }

    static std::list<key_type> keys(Map const& self)
    {
        std::list<key_type> t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.push_back(it->first);
        return t;
    }
    static std::list<mapped_type> values(Map const& self)
    {
        std::list<mapped_type> t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.push_back(it->second);
        return t;
    }
    static std::list<std::tr1::tuple<key_type, mapped_type> > items(Map const& self)
    {
        std::list<std::tr1::tuple<key_type, mapped_type> > t;
        for(typename Map::const_iterator it=self.begin(); it!=self.end(); ++it)
            t.push_back( std::tr1::make_tuple(it->first, it->second) );
        return t;
    }
};


#endif

