#ifndef COMPOSITE_FILTER_H
#define	COMPOSITE_FILTER_H

#include "pointers.h"
#include "filter.h"

class composite_filter_class : public filter_class
{
public:
    composite_filter_class();

    virtual filter_p create_filter();

    static const char* get_id();
};

class composite_filter : public filter
{
public:
    enum operation_t
    {
        op_and,
        op_or
    };

    composite_filter(filter_class_p fc);

    operation_t operation() const {return operation_;}
    void set_opertaion(operation_t op);

    void add_filter(filter_p f);
    void remove_filter(filter_p f);
    const filters_t& filters() const {return filters_;}

    virtual bool filter_server(const server_info& si);

private:
    operation_t operation_;
    filters_t filters_;
};

#endif	/* COMPOSITE_FILTER_H */

