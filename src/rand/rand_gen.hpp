/*
    (C) Konstantin J. Chernov
    See LICENSE for information about licensing
*/

#ifndef _RAND_GEN_HPP
#define _RAND_GEN_HPP

#include <iostream>
#include "../blowfish/blowfish.hpp"

class Rand_Gen
{
    public:
        static Rand_Gen* GetInstance()
        {
            if (_instance == NULL)
            {
                return new Rand_Gen;
            }
            return _instance;
        }

        void SetSalt(std::string salt);

        std::string Get(size_t bytes);

    private:
        Rand_Gen()
        {
            _instance = this;
        };
        ~Rand_Gen()
        {
            _instance = NULL;
        };

        std::string _salt;
        std::string _curr;
        size_t _pos;
        Blowfish _blow;

        static Rand_Gen* _instance;
};

#endif
