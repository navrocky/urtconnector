
#ifndef URT_PYTHON_ENGINE_H
#define URT_PYTHON_ENGINE_H

#include <boost/exception/info.hpp>

#include <QObject>

#include <common/server_bookmark.h>

#include "python_api.h"

    typedef boost::error_info<struct engine_error, std::string> engine_err;        

class engine : public QObject{
    Q_OBJECT
public:

    engine(server_bookmark_list_p bookmarks, python_ctx& ctx)
        : ctx_(ctx)
    {
        connect(bookmarks.get(), SIGNAL(changed(server_bookmark, server_bookmark)), SLOT(changed(server_bookmark, server_bookmark)));
    }
    
    virtual ~engine(){};
    
public Q_SLOTS:
    void changed( const server_bookmark& old_bm, const server_bookmark& new_bm ) {
        
        try {
            boost::python::object func = ctx_.main_namespace["bookmark_changed"];
            func(boost::python::object(old_bm), boost::python::object(new_bm));
        }
        catch(boost::python::error_already_set const &)
        {
            throw boost::enable_error_info(std::runtime_error(python_err(ctx_))) << engine_err("bookmark_cahnged failed");
        }
        catch(boost::exception& e)
        {
            e << engine_err("bookmark_cahnged failed");
            throw;
        }    
    }
    
private:
    python_ctx& ctx_;
    
};


#endif

