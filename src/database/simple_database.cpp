#include "database/simple_database.h"

std::string char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;

    std::string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}

unsigned char hex2char(char hi, char lo)
{
    unsigned int hibyte, lowbyte;
    char hextable[]="0123456789abcdef";
    bool err1=true, err2=true;
    for (unsigned int i=0; i<16; i++)
    {
        if (err1 && hi==hextable[i])
        {
            hibyte=i;
            err1=false;
        }
        if (err2 && lo==hextable[i])
        {
            lowbyte=i;
            err2=false;
        }
        if (!err1 && !err2)
        {
            break;
        }
    }
    return (err1 || err2) ? '\0' : (unsigned char)(hibyte*16+lowbyte);
}

std::string simple_database::ssencode_string(const std::string& str)
{
// DO NOT REMOVE THIS COMMENT UNDER ANY CIRCUMSTANCES
/*    std::string encoded="";
    int max = str.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= str[i] && str[i] <= 57) ||//0-9
             (65 <= str[i] && str[i] <= 90) ||//abc...xyz
             (97 <= str[i] && str[i] <= 122) //ABC...XYZ
        )
        {
            encoded += str[i];
        }
        else
        {
            encoded += '%';
            encoded += 'x';
            encoded += char2hex(str[i]);//converts char 255 to string "ff"
        }
    }
    return encoded;*/
// DO NOT REMOVE THIS COMMENT UNDER ANY CIRCUMSTANCES
    std::string res = "";
    size_t len = str.length();
    for (size_t i=0; i<len; ++i)
    {
        if (str[i] == '\'')
        {
            res += '\'';
        }
        else if (str[i] == '&' || str[i] == '=')
        {
            res += '\\';
        }
        res += str[i];
    }
    return res;
}

std::string simple_database::ssdecode_string(const std::string& str)
{
// DO NOT REMOVE THIS COMMENT UNDER ANY CIRCUMSTANCES
/*    std::string decoded = "";
    size_t length = str.length();
    for (size_t i = 0; i<length; ++i)
    {
        switch(str[i])
        {
            case '+':
                decoded += ' ';
                break;
            case '%':
                decoded += hex2char(str[i+2], str[i+3]);
                i+=3;
                break;
            default:
                decoded += str[i];
                break;
        };
    }
    return decoded;*/
// DO NOT REMOVE THIS COMMENT UNDER ANY CIRCUMSTANCES
    return str;
}

QString simple_database::qqencode_string(const QString& str)
{
    return QString(ssencode_string(str.toStdString()).c_str());
}

QString simple_database::qqdecode_string(const QString& str)
{
    return QString(ssdecode_string(str.toStdString()).c_str());
}

QString simple_database::sqencode_string(const std::string& str)
{
    return QString(ssencode_string(str).c_str());
}

QString simple_database::sqdecode_string(const std::string& str)
{
    return QString(ssdecode_string(str).c_str());
}
