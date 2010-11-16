#pragma once

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

template <class Body>
class implicit_sharing
{
public:
    implicit_sharing()
    : body_(boost::make_shared<Body>())
    {
    }

    /*! Call this from all non const methods. */
    void detach()
    {
        if (body_.unique())
            return;
        boost::shared_ptr<Body> old = body_;
        body_ = boost::make_shared<Body>(*(body_.get()));
    }

    /*! Access to shared body. */
    inline Body* operator->() const
    {
        return body_.get();
    }

    inline Body* operator->()
    {
        detach();
        return body_.get();
    }

private:
    boost::shared_ptr<Body> body_;
};
