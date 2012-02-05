
#ifndef URT_PYTHON_ENGINE_H
#define URT_PYTHON_ENGINE_H

#include <QObject>

#include <common/server_bookmark.h>

#include "python_api.h"

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
        } catch(...){
            std::cerr<<"Exception!"<<std::endl;
            PyErr_Print();
        }
    }
    
private:
    python_ctx& ctx_;
    
};


#endif

