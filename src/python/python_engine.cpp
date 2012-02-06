
#include <boost/exception/diagnostic_information.hpp>

#include <QLineEdit>

#include "python_engine.h"





QLineEdit* le;

engine::engine(server_bookmark_list_p bookmarks, python_ctx& ctx, main_tab_widget* tw)
    : ctx_(ctx)
    , tw_(tw)
{
        connect(bookmarks.get(), SIGNAL(changed(server_bookmark, server_bookmark)), SLOT(changed(server_bookmark, server_bookmark)));

        le = new QLineEdit(0);
        connect (le, SIGNAL(returnPressed()), SLOT(exec()));
        le->show();
}

void engine::changed(const server_bookmark& old_bm, const server_bookmark& new_bm)
{
    
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

void engine::exec()
{
    
        try {
            python_exec(le->text().toStdString(), ctx_);
        }
        catch(std::exception& e) {
            std::cerr<<"ENGINE: can't eval: " << le->text().toStdString()<<std::endl;
            if (boost::exception* be = boost::current_exception_cast<boost::exception>())
            {
                std::cerr << boost::diagnostic_information(*be);
            }
            else
            {
                std::cerr << e.what();
            }
        }
        catch(...)
        {
            std::cerr<<"ENGINE: can't eval: " << le->text().toStdString()<<std::endl;
        }    
}

void engine::add_tab(QWidget* tab)
{
    tw_->add_widget(tab);
}

    
