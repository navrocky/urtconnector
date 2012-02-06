
#ifndef URT_PYTHON_ENGINE_H
#define URT_PYTHON_ENGINE_H

#include <boost/exception/info.hpp>

#include <QObject>

#include <common/server_bookmark.h>
#include <common/main_tab_widget.h>

#include "python_api.h"


    typedef boost::error_info<struct engine_error, std::string> engine_err;        

class engine : public QObject{
    Q_OBJECT
public:

    engine(server_bookmark_list_p bookmarks, python_ctx& ctx, main_tab_widget* tw);
    virtual ~engine(){};
    
    void add_tab(QWidget* tab);
    
public Q_SLOTS:
    void changed( const server_bookmark& old_bm, const server_bookmark& new_bm );

private Q_SLOTS:
    void exec();
    
private:
    python_ctx& ctx_;
    main_tab_widget* tw_;
    
};


#endif

