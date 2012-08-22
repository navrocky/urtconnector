#pragma once

#include <memory>

class profile
{
public:
    static remote::group get_group();
    static void set_group(const remote::group& remote);   
    
    profile();
};