/*
    DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
    Version 2, December 2004

    Copyright (C) 2004 Sam Hocevar

    14 rue de Plaisance, 75014 Paris, France
    Everyone is permitted to copy and distribute verbatim or modified
    copies of this license document, and changing it is allowed as long
    as the name is changed.

    DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
    TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

    0. You just DO WHAT THE FUCK YOU WANT TO.
*/

/*
    This implementation of Blowfish alghoritm made by
    Konstantin J. Chernov.
    Website: http://k.cher.pp.ru

    Description of alghoritm was found here:
    http://www.schneier.com/paper-blowfish-fse.html

    Also, this implementation is using CFB (Cipher feedback) mode.
    Description of CFB was found here:
    http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation

    As initializing vector I choosed the first block of plaintext.
    CFB is used only in EncryptString and DecryptString. Further
    description you can find in comments.

    I'm not sure whether this implementation will work correctly on
    big-endian architectures. It was written to run on little-endian.
*/

#include <iostream>
#include "blowfish.hpp"
#include "pi.h"

Blowfish::Blowfish()
{
    Reset();
}
Blowfish::~Blowfish()
{
}

/*
    This function is setting a password and generating
    subkeys.
*/
void Blowfish::SetPassword(std::string password)
{
    /* Resetting sboxes and parray to it's default settings */
    Reset();
    size_t length = password.length();
    /* XOR-ing pboxes to 4-bytes blocks of password */
    for (size_t i=0; i<_NUM_SUBKEYS; ++i)
    {
        WORD c;
        /* In case if we dont have enough password length,
        we can just repeat it */
        for (size_t j=0; j<4; ++j)
        {
            c.c[j] = password[(4*i+j)%length];
        }
        /* Reordering bytes of word if we have big endian arch */
        OrderBytes(c);
        /* And XOR-ing */
        m_pbox[i] ^= c.i;
    }
    /* Repeatedly encrypting zero-block (block of 4 bytes, each byte is 0)
        and then filling pboxes and sboxes */
    WORD zeroL, zeroR;
    zeroL.i = zeroR.i = 0;
    for (size_t i=0; i<_NUM_SUBKEYS; i+=2)
    {
        EncryptBlock(zeroL,zeroR);
        m_pbox[i] = zeroL.i;
        m_pbox[i+1] = zeroR.i;
    }

    for (size_t i=0; i<4; ++i)
    {
        for (size_t j=0; j<256; j+=2)
        {
            EncryptBlock(zeroL, zeroR);
            m_sbox[i][j] = zeroL.i;
            m_sbox[i][j+1] = zeroR.i;
        }
    }
}

/*
    This function is encrypting string passed as 'from'
    and puts ciphertext to string passed as 'to'.
    It returns -1 on error and 0 if everything is OK.
*/
int Blowfish::EncryptString(std::string from, std::string &to)
{
    /* Try is used because string.insert can throw an exception */
    try
    {
        /* We're doing _NUM_TRIES cycles of ciphering to improve
            ciphertext length  */
        for (size_t i=0; i<_NUM_TRIES; ++i)
        {
            /* Length of input string */
            size_t length = from.length();
            /* How many characters do we need to add to have 8N bytes */
            size_t addlength = length % 8;
            /* If we already have 8N bytes, add another 8 to be able to
                write amount of added bytes in last byte */
            addlength = (addlength == 0) ? 8 : (8-addlength);
            /* Adding first addlength-1 bytes of string */
            for (size_t i=0; i<addlength-1; ++i)
            {
                from.insert(length+i, 1, from[i % length]);
            }
            /* In the last byte we storing how many bytes we have added */
            from.insert(length+addlength-1, 1, addlength);
            length += addlength;
            /* Working with unsigned char further */
            unsigned char* str = (unsigned char*)from.c_str();
            for (size_t i=0; i<length; i+=8)
            {
                /* Left part of data block */
                WORD *left = (WORD*)(&str[i]);
                /* Right part of data block */
                WORD *right = (WORD*)(&str[i+4]);
                /* Reordering bytes */
                OrderBytes(*left);
                OrderBytes(*right);
                if (i>=8)
                {
                    /* Using CFB. Description can be found in Wikipedia */
                    WORD prev_left = *(WORD*)(&str[i-8]);
                    WORD prev_right = *(WORD*)(&str[i-4]);
                    OrderBytes(prev_left);
                    OrderBytes(prev_right);
                    EncryptBlock(prev_left, prev_right);
                    left->i ^= prev_left.i;
                    right->i ^= prev_right.i;
                    OrderBytes(prev_right);
                    OrderBytes(prev_left);
                }
                else
                {
                    /* First step. No need in CFB */
                    EncryptBlock(*left, *right);
                }
                /* Getting bytes back in order */
                OrderBytes(*right);
                OrderBytes(*left);
            }
            /* Changing input string to ciphered string */
            for (size_t i=0; i<length; ++i)
            {
                from[i] = str[i];
            }
        }
        /* When encrypting is done, applying changes to output string */
        to = from;
        return 0;
    }
    catch (...)
    {
        return -1;
    }
}

/*
    This function is decrypting string.
    There's no need to describe how it works, because
    almost everything was described in EncryptString.
    It returns -1 on error, 0 if everything is OK.
*/
int Blowfish::DecryptString(std::string from, std::string &to)
{
    try
    {
        for (size_t i=0; i<_NUM_TRIES; ++i)
        {
            size_t length = from.length();
            if (length%8 != 0)
            {
                return -1;
            }
            unsigned char* str = (unsigned char*)from.c_str();
            unsigned char* str_cipher = new unsigned char[length];
            for (size_t i=0; i<length; ++i)
            {
                str_cipher[i] = str[i];
            }
            for (size_t i=0; i<length; i+=8)
            {
                WORD *left = (WORD*)(&str[i]);
                WORD *right = (WORD*)(&str[i+4]);
                OrderBytes(*left);
                OrderBytes(*right);
                if (i>=8)
                {
                    WORD prev_left = *(WORD*)(&str_cipher[i-8]);
                    WORD prev_right = *(WORD*)(&str_cipher[i-4]);
                    OrderBytes(prev_left);
                    OrderBytes(prev_right);
                    EncryptBlock(prev_left, prev_right);
                    left->i ^= prev_left.i;
                    right->i ^= prev_right.i;
                    OrderBytes(prev_right);
                    OrderBytes(prev_left);
                }
                else
                {
                    DecryptBlock(*left, *right);
                }
                OrderBytes(*right);
                OrderBytes(*left);
            }
            for (size_t i=0; i<length; ++i)
            {
                from[i] = str[i];
            }
            from.erase(length-from[length-1],from[length-1]);
        }
        to = from;
        return 0;
    }
    catch (...)
    {
        return -1;
    }
}

/*
    Resetting sboxes and parray to it's default settings.
*/
void Blowfish::Reset()
{
    for (size_t i=0; i<256; ++i)
    {
        m_sbox[0][i] = sbox0[i];
    }
    for (size_t i=0; i<256; ++i)
    {
        m_sbox[1][i] = sbox1[i];
    }
    for (size_t i=0; i<256; ++i)
    {
        m_sbox[2][i] = sbox2[i];
    }
    for (size_t i=0; i<256; ++i)
    {
        m_sbox[3][i] = sbox3[i];
    }
    for (size_t i=0; i<_NUM_ROUNDS+2; ++i)
    {
        m_pbox[i] = parray[i];
    }
}

/*
    On little-endian arch we have DCBA-order of
    bytes in memory. But not for array of characters.
    That's why we need to be able to reorder it from ABCD (char*)
    to DCBA (unsigned int).
*/
void Blowfish::OrderBytes(WORD &data)
{
#ifdef LITTLE_ENDIAN
    unsigned char c[4];
    for (size_t i=0; i<4; ++i)
    {
        c[i] = data.c[i];
    }
    for (size_t i=0; i<4; ++i)
    {
        data.c[i] = c[3-i];
    }
#endif
}

/*
    F-function. Description can be found in description of algorithm.
*/
unsigned int Blowfish::F(WORD w)
{
    OrderBytes(w);
    return (((m_sbox[0][w.c[0]] + m_sbox[1][w.c[1]]) ^ m_sbox[2][w.c[2]])+m_sbox[3][w.c[3]]);
}

/*
    This function encrypts a 8-bytes block of data.
    It's quite simple.
*/
void Blowfish::EncryptBlock(WORD &l, WORD &r)
{
    unsigned int temp;
    for (size_t i=0; i<_NUM_ROUNDS; ++i)
    {
        l.i ^= m_pbox[i];
        r.i = F(l) ^ r.i;
        temp = l.i;
        l.i = r.i;
        r.i = temp;
    }
    temp = l.i;
    l.i = r.i;
    r.i = temp;
    r.i ^= m_pbox[_NUM_ROUNDS];
    l.i ^= m_pbox[_NUM_ROUNDS+1];
}

/*
    This function decrypts a 8-bytes block of data.
    It's quite simple.
*/
void Blowfish::DecryptBlock(WORD &l, WORD &r)
{
    unsigned int temp;
    for (size_t i=0; i<_NUM_ROUNDS; ++i)
    {
        l.i ^= m_pbox[_NUM_ROUNDS+1-i];
        r.i = F(l) ^ r.i;
        temp = l.i;
        l.i = r.i;
        r.i = temp;
    }
    temp = l.i;
    l.i = r.i;
    r.i = temp;

    r.i ^= m_pbox[1];
    l.i ^= m_pbox[0];
}
