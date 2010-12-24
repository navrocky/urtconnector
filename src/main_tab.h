
#ifndef URT_MAIN_TAB_H
#define URT_MAIN_TAB_H

#include "boost/utility.hpp"

#include <common/server_id.h>
#include <filters/pointers.h>

#include "pointers.h"

class QMainWindow;

///Class that provide base interface for any widget at main tab widget
class main_tab : boost::noncopyable {

protected:
    ///this fuction MUST be called from derived class 
    void init_main_tab( QMainWindow* self, filter_factory_p factory = filter_factory_p() );
    
    void init_filter_toolbar();

public:

    virtual ~main_tab();

    ///returns currently selected server if any
    virtual server_id selected_server() const;

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const;

    void force_servers_update();

    const filter_list& filterlist() const;
    
protected:
    ///this function automatically called when server_list has some changes
    virtual void servers_updated() = 0;
    virtual void filter_changed() {};

private:
    void update_toolbar_filter();
    void edit_filter();
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};


#endif

