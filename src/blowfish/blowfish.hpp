#ifndef _BLOWFISH_HPP
#define _BLOWFISH_HPP

#include <iostream>

#define _NUM_ROUNDS 16
#define _NUM_SUBKEYS (_NUM_ROUNDS+2)
#define _NUM_TRIES 50

class Blowfish
{
    union WORD
    {
        unsigned char c[4];
        unsigned int i;
    };

    public:
        Blowfish();
        ~Blowfish();

        void SetPassword(std::string password);
        int EncryptString(std::string from, std::string &to);
        int DecryptString(std::string from, std::string &to);

    private:
        void Reset();
        void OrderBytes(WORD &data);
        unsigned int F(WORD w);
        void EncryptBlock(WORD &l, WORD &r);
        void DecryptBlock(WORD &l, WORD &r);

        size_t m_sbox[4][256];
        size_t m_pbox[_NUM_ROUNDS+2];
};

#endif
