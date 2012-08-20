#pragma once




class profile
{
public:
    static remote::group get_group();
    static void set_group(const remote::group& remote);   
    
    profile();
};