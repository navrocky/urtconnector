/*
    (C) Konstantin J. Chernov
    See LICENSE for information about licensing
*/

#include "rand_gen.hpp"

void Rand_Gen::SetSalt(std::string salt)
{
    _salt = salt;
    _blow.SetPassword(salt);
    _curr = salt;
//    _pos = 0;
}

std::string Rand_Gen::Get(size_t bytes)
{
    size_t length = _curr.length();
    for ( ; ; )
    {
        while (length < bytes + _pos)
        {
            _blow.EncryptString(_curr,_curr);
            length = _curr.length();
            _pos = 0;
        }
        std::string temp = _curr.substr(_pos,bytes);
        _pos += bytes;
        for (size_t k=0; k<bytes; ++k)
        {
            temp[k] = (unsigned int)temp[k] % 10 + '0';
        }
        if (temp[0] == '0')
        {
            continue;
        }
        return temp;
    }
}
