#include <algorithm>
#include <cstdio>
#include <list>
#include <map>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <memory>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <string_view>
#include <sys/types.h>
#include <sys/stat.h>

#include "datetime.h"
#include "urlcode.h"

#ifdef WIN32
#define stat _stat
#endif

#include "func.h"
#include "cost_define.h"

namespace http
{
namespace fs = std::filesystem;
bool str_cmp(std::string_view str1, std::string_view str2)
{
    if (str1.size() != str2.size())
    {
        return false;
    }
    for (unsigned int i = 0; i < str1.size(); i++)
    {
        if (str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}
bool str_casecmp(std::string_view str1, std::string_view str2)
{
    if (str1.size() != str2.size())
    {
        return false;
    }
    for (unsigned int i = 0; i < str1.size(); i++)
    {
        if (str1[i] != str2[i])
        {
            if (str1[i] < 91 && str1[i] > 64)
            {
                if ((str1[i] + 32) == str2[i])
                {
                    continue;
                }
            }
            else if (str2[i] < 91 && str2[i] > 64)
            {
                if (str1[i] == (str2[i] + 32))
                {
                    continue;
                }
            }
            return false;
        }
    }
    return true;
}
bool str_cmp_pre(std::string_view str1, std::string_view str2, unsigned int length)
{
    if (length == 0)
    {
        length = str2.size();
    }
    if (str1.size() < str2.size())
    {
        return false;
    }
    for (unsigned int i = 0; i < length; i++)
    {
        if (i < str2.size())
        {
            if (str1[i] == str2[i])
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    return true;
}
bool str_casecmp_pre(std::string_view str1, std::string_view str2, unsigned int length)
{
    if (length == 0)
    {
        length = str2.size();
    }
    if (str1.size() < str2.size())
    {
        return false;
    }
    for (unsigned int i = 0; i < length; i++)
    {
        if (i < str2.size())
        {
            if (str1[i] == str2[i])
            {
                continue;
            }
            else
            {
                if (str1[i] < 91 && str1[i] > 64)
                {
                    if ((str1[i] + 32) == str2[i])
                    {
                        continue;
                    }
                }
                else if (str2[i] < 91 && str2[i] > 64)
                {
                    if (str1[i] == (str2[i] + 32))
                    {
                        continue;
                    }
                }
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    return true;
}
bool str_cmp_last(std::string_view str1, std::string_view str2, unsigned int length)
{
    if (length == 0)
    {
        length = str2.size();
    }
    int a = (int)str1.size();
    int b = (int)str2.size();
    if (a == 0)
    {
        return false;
    }
    else if (b == 0)
    {
        return false;
    }
    if (a < b)
    {
        return false;
    }
    a = a - 1;
    b = b - 1;

    for (unsigned int i = 0; i < length; i++)
    {
        a -= i;
        b -= i;
        if (b < 0)
        {
            return true;
        }
        if (str1[a] == str2[b])
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string mb_reverse(std::string_view str)
{
    std::string temp;
    if (str.size() == 0)
    {
        return temp;
    }
    int i = str.size() - 1;
    unsigned int j = 0;
    for (; i >= 0; i--)
    {
        unsigned char cc = str[i];
        if (cc < 0x80)
        {
            temp.push_back(str[i]);
        }
        else
        {
            j = i;
            for (; i >= 0; i--)
            {
                unsigned char dd = str[i];
                if (dd & 0x40)
                {
                    if (i > 2)
                    {
                        //emoji 0x200D
                        //https://unicode.org/Public/emoji/5.0/emoji-data.txt
                        dd               = str[i - 1];
                        unsigned char ee = str[i - 2];
                        unsigned char ff = str[i - 3];
                        if (ff == 0xE2 && ee == 0x80 && dd == 0x8D)
                        {
                            i = i - 3;
                            //1F3FB..1F3FF  ; Emoji_Modifier       #  8.0  [5] (🏻..🏿)    light skin tone..dark skin tone

                            // if(i>3)
                            // {
                            //     cc = str[i-1];
                            //     dd = str[i-2];
                            //     ee = str[i-3];
                            //     ff = str[i-4];
                            //     //F09F8FBB
                            //     if (cc == 0xBB && dd == 0x8F && ee == 0x9F && ff == 0xF0 )
                            //     {
                            //         i = i - 4;
                            //     }
                            //     else if (cc == 0xBC && dd == 0x8F && ee == 0x9F && ff == 0xF0 )
                            //     {
                            //         i = i - 4;
                            //     }
                            //     else if (cc == 0xBD && dd == 0x8F && ee == 0x9F && ff == 0xF0 )
                            //     {
                            //         i = i - 4;
                            //     }
                            //     else if (cc == 0xBE && dd == 0x8F && ee == 0x9F && ff == 0xF0 )
                            //     {
                            //         i = i - 4;
                            //     }
                            //     else if (cc == 0xBF && dd == 0x8F && ee == 0x9F && ff == 0xF0 )
                            //     {
                            //         i = i - 4;
                            //     }
                            // }
                            continue;
                        }
                        else if (ff == 0xE2 && ee == 0x80 && dd == 0x8B) 
                        {
                            //Used for separating longer words with line breaks
                            i = i - 3;
                            continue;
                        }
                        else if (ff == 0xE2 && ee == 0x80 && dd == 0x8C) 
                        {
                            //Arabic, German, Hindi
                            i = i - 3;
                            continue;
                        }
                        else if (ff == 0xE2 && ee == 0x80 && dd == 0x8E) 
                        {
                            //Right to Left
                            i = i - 3;
                            continue;
                        }
                        else if (ff == 0xE2 && ee == 0x80 && dd == 0x8F) 
                        {
                            //Right to Left
                            i = i - 3;
                            continue;
                        }
                    }
                    //May be check colour of skin
                    for (unsigned int k = i; k <= j; k++)
                    {
                        temp.push_back(str[k]);
                    }
                    break;
                }
                if (i == 0)
                {
                    break;
                }
            }
        }
        if (i == 0)
        {
            break;
        }
    }
    return temp;
}

bool str_casecmp_last(std::string_view str1, std::string_view str2, unsigned int length)
{
    if (length == 0)
    {
        length = str2.size();
    }
    int a = (int)str1.size();
    int b = (int)str2.size();
    if (a == 0)
    {
        return false;
    }
    else if (b == 0)
    {
        return false;
    }
    if (a < b)
    {
        return false;
    }
    a = a - 1;
    b = b - 1;

    for (unsigned int i = 0; i < length; i++)
    {
        a -= i;
        b -= i;
        if (b < 0)
        {
            return true;
        }
        if (str1[a] == str2[b])
        {
            continue;
        }
        else
        {
            if (str1[a] < 91 && str1[a] > 64)
            {
                if ((str1[a] + 32) == str2[b])
                {
                    continue;
                }
            }
            else if (str2[b] < 91 && str2[b] > 64)
            {
                if (str1[a] == (str2[b] + 32))
                {
                    continue;
                }
            }
            return false;
        }
    }
    return true;
}
void get_filename(const std::string &filename, std::string &filename_name, std::string &filename_ext)
{

    int j = filename.size() - 1;
    if (j == -1)
        return;
    for (; j >= 0; j--)
    {
        if (filename[j] == '.')
        {
            if (filename_name.size() == 2 && filename_name[0] == 'z' && filename_name[1] == 'g')
            {
                filename_name.push_back('.');
                continue;
            }
            filename_ext = filename_name;
            filename_name.clear();
            continue;
        }
        if (filename[j] == '/')
        {
            j--;
            break;
        }
        filename_name.push_back(filename[j]);
    }
    std::reverse(filename_name.begin(), filename_name.end());
    std::reverse(filename_ext.begin(), filename_ext.end());
}
std::string get_filename(const std::string &filename)
{
    std::string filename_name;
    if (filename.empty())
    {
        return "";
    }
    int j = filename.size() - 1;
    if (j == -1)
        return "";
    for (; j >= 0; j--)
    {
        if (filename[j] == '.')
        {
            if (filename_name.size() == 2 && filename_name[0] == 'z' && filename_name[1] == 'g')
            {
                continue;
            }
            filename_name.clear();
            continue;
        }
        if (filename[j] == '/')
        {
            j--;
            break;
        }
        filename_name.push_back(filename[j]);
    }
    std::reverse(filename_name.begin(), filename_name.end());
    return filename_name;
}
std::string get_fileext(const std::string &filename)
{
    unsigned int filename_length = filename.size();
    std::string temp;
    if (filename_length == 0)
    {
        return temp;
    }
    unsigned int j = 0;
    for (unsigned int i = filename_length - 1; i > 0; i--)
    {
        if (filename[i] == '.')
        {
            break;
        }
        j++;
        if (j > 6)
        {
            j = 0;
            break;
        }
    }
    if (j == 0)
    {
        return temp;
    }
    for (unsigned int i = filename_length - j; i < filename_length; i++)
    {
        temp.push_back(filename[i]);
    }
    return temp;
}
std::vector<std::string> mb_split(std::string_view pattern, std::string_view msg)
{
    std::vector<std::string> temp;
    std::string mp;
    unsigned int j = 0;
    unsigned int n = msg.size();

    for (unsigned int i = 0; i < n; i++)
    {
        for (j = 0; j < pattern.size(); j++)
        {
            if ((i + j) >= n)
            {
                break;
            }
            if (pattern[j] != msg[i + j])
            {
                break;
            }
        }
        if (j == pattern.size())
        {
            temp.emplace_back(mp);
            mp.clear();
            i = i + j - 1;
            continue;
        }
        mp.push_back(msg[i]);
    }
    if (mp.size() > 0)
    {
        temp.emplace_back(mp);
    }
    return temp;
}
unsigned int mb_strlen(std::string_view str)
{
    unsigned int length    = 0;
    unsigned long long pos = 0;

    for (; pos < str.size(); pos++)
    {
        unsigned char c = (unsigned char)str[pos];
        if (c < 0x80)
        {
            length++;
        }
        else if (c < 0xC0)
        {
            length++;
        }
        else if (c >= 0xC0 && c < 0xE0)
        {
            pos += 1;
            length++;
        }
        else if (c >= 0xE0 && c < 0xF0)
        {
            pos += 2;
            length++;
        }
        else if (c >= 0xF0 && c < 0xF8)
        {
            pos += 3;
            length++;
        }
        else
        {
            length++;
        }
    }
    return length;
}
bool check_isodate(std::string_view filename)
{
    if (filename.size() < 10)
    {
        return false;
    }
    for (unsigned int i = 0; i < 4; ++i)
    {
        if (filename[i] > 0x2F && filename[i] < 0x3A)
        {
            continue;
        }
        return false;
    }
    if (filename[4] != '-')
    {
        return false;
    }
    for (unsigned int i = 5; i < 7; ++i)
    {
        if (filename[i] > 0x2F && filename[i] < 0x3A)
        {
            continue;
        }
        return false;
    }
    if (filename[7] != '-')
    {
        return false;
    }
    for (unsigned int i = 8; i < 10; ++i)
    {
        if (filename[i] > 0x2F && filename[i] < 0x3A)
        {
            continue;
        }
        return false;
    }
    if (filename.size() > 10)
    {
        return false;
    }
    return true;
}
std::string strip_domainname(std::string_view domainname)
{
    std::string temp;
    for (unsigned int i = 0; i < domainname.size(); ++i)
    {
        if (domainname[i] > 0x2F && domainname[i] < 0x3A)
        {
            temp.push_back(domainname[i]);
        }
        else if (domainname[i] > 0x40 && domainname[i] < 0x5B)
        {
            temp.push_back(domainname[i] + 32);
        }
        else if (domainname[i] > 0x60 && domainname[i] < 0x7B)
        {
            temp.push_back(domainname[i]);
        }
        else if (domainname[i] == '.')
        {
            if (i > 0)
            {
                if (domainname[i - 1] == '-')
                {
                    temp[temp.size() - 1] = '.';
                }
                else
                {
                    temp.push_back('.');
                }
            }
        }
        else if (domainname[i] == '-')
        {
            if (i > 0)
            {
                if (domainname[i - 1] != '-')
                {
                    temp.push_back('-');
                }
            }
        }
    }
    if (temp[temp.size() - 1] == '-')
    {
        temp.resize(temp.size() - 1);
    }
    return temp;
}
bool tidy_domainname(std::string &domainname)
{
    for (unsigned int i = 0; i < domainname.size(); ++i)
    {
        if (domainname[i] > 0x2F && domainname[i] < 0x3A)
        {
        }
        else if (domainname[i] > 0x40 && domainname[i] < 0x5B)
        {
            domainname[i] = domainname[i] + 32;
        }
        else if (domainname[i] > 0x60 && domainname[i] < 0x7B)
        {
        }
        else if (domainname[i] == '.')
        {
        }
        else if (domainname[i] == '-')
        {
        }
        else
        {
            return false;
        }
    }
    return true;
}
bool is_domainname(std::string_view domainname)
{
    if (domainname.empty())
    {
        return false;
    }
    if (domainname[0] == '-' || domainname[domainname.size() - 1] == '-')
    {
        return false;
    }
    if (domainname[0] == '.')
    {
        return false;
    }

    for (unsigned int i = 0; i < domainname.size(); ++i)
    {
        if (domainname[i] > 0x2F && domainname[i] < 0x3A)
        {
        }
        else if (domainname[i] > 0x40 && domainname[i] < 0x5B)
        {
        }
        else if (domainname[i] > 0x60 && domainname[i] < 0x7B)
        {
        }
        else if (domainname[i] == '.')
        {
            unsigned int j = i + 1;
            if (j < domainname.size() && domainname[j] == '-')
            {
                return false;
            }
            if (i > 0)
            {
                if (domainname[i - 1] == '-')
                {
                    return false;
                }
            }
        }
        else if (domainname[i] == '-')
        {
        }
        else
        {
            return false;
        }
    }
    return true;
}
std::string mb_substr(std::string_view str, int begin, int length)
{
    std::string temp;
    int strlength = 0;
    // int spacenum = 0;
    unsigned char c;
    if (begin < 0)
    {
        if (length == 0)
        {
            strlength = mb_strlen(str);
            int n     = strlength + begin;
            if (n > strlength)
            {
                n = strlength;
            }
            if (n < 0)
            {
                n = 0;
            }

            int offsetnum = 0;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
            }

            return temp;
        }
        else if (length < 0)
        {
            strlength = mb_strlen(str);
            int j     = strlength + length;
            if (j > strlength)
            {
                j = strlength;
            }
            if (j < 0)
            {
                j = 0;
            }
            int n = strlength + begin;
            if (n > strlength)
            {
                n = strlength;
            }
            if (n < 0)
            {
                n = 0;
            }

            if (n >= j)
            {
                return temp;
            }
            int offsetnum = 0;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                if (offsetnum >= j)
                {
                    break;
                }
            }
            return temp;
        }
        else
        {
            strlength = mb_strlen(str);

            int n = strlength + begin;
            if (n > strlength)
            {
                n = strlength;
            }
            if (n < 0)
            {
                n = 0;
            }

            int j = n + length;
            if (j > strlength)
            {
                j = strlength;
            }
            if (j < 0)
            {
                j = 0;
            }
            if (n >= j)
            {
                return temp;
            }

            int offsetnum = 0;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                if (offsetnum >= j)
                {
                    break;
                }
            }

            return temp;
        }
    }
    else
    {
        if (length == 0)
        {

            int offsetnum = 0;
            int n         = begin;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
            }

            return temp;
        }
        else if (length < 0)
        {
            strlength = mb_strlen(str);
            int j     = strlength + length;
            if (j < 0)
            {
                j = 0;
            }
            if (begin > strlength)
            {
                begin = strlength;
            }

            int n = j;
            j     = begin;

            int offsetnum = 0;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                if (offsetnum >= j)
                {
                    break;
                }
            }

            return temp;
        }
        else
        {
            strlength = mb_strlen(str);

            if (begin > strlength)
            {
                begin = strlength;
            }
            int j = begin + length;
            if (j > strlength)
            {
                j = strlength;
            }
            int n         = begin;
            int offsetnum = 0;
            for (unsigned int pos = 0; pos < str.size(); pos++)
            {
                c = (unsigned char)str[pos];
                if (c < 0x80)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c < 0xC0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                else if (c >= 0xC0 && c < 0xE0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                    }
                    pos += 1;
                    offsetnum++;
                }
                else if (c >= 0xE0 && c < 0xF0)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                    }
                    pos += 2;
                    offsetnum++;
                }
                else if (c >= 0xF0 && c < 0xF8)
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                        temp.push_back(str[pos + 1]);
                        temp.push_back(str[pos + 2]);
                        temp.push_back(str[pos + 3]);
                    }
                    pos += 3;
                    offsetnum++;
                }
                else
                {
                    if (offsetnum >= n)
                    {
                        temp.push_back(str[pos]);
                    }
                    offsetnum++;
                }
                if (offsetnum >= j)
                {
                    break;
                }
            }
            return temp;
        }
    }

    return temp;
}

struct stat filestat(std::string &file_name)
{
    struct stat finfo;
    if (stat(file_name.c_str(), &finfo) == 0)
    {
    }
    return finfo;
}
std::map<std::string, std::string> filepath(std::string &str)
{
    std::map<std::string, std::string> temp;
    std::vector<std::string> vpath;
    bool isext     = false;
    bool isurl     = false;
    unsigned int i = 0;
    for (; i < str.size(); i++)
    {
        if (str[i] == 0x20 || str[i] == 0x09 || str[i] == 0x0D || str[i] == 0x0A)
        {
            continue;
        }
        break;
    }
    if (i < str.size() && str[i] == 'h')
    {
        if (((i + 6) < str.size()) && str[i + 1] == 't' && str[i + 2] == 't' && str[i + 3] == 'p' && str[i + 4] == ':' &&
            str[i + 5] == '/' && str[i + 6] == '/')
        {
            temp["scheme"] = "http";
            bool isip6     = false;
            bool isport    = false;
            std::string temphost;
            std::string tempport;
            unsigned int j = i + 7;
            for (; j < str.size(); j++)
            {
                if (str[j] == '/')
                {
                    break;
                }
                if (str[j] == '[')
                {
                    isip6 = true;
                    continue;
                }
                if (str[j] == ']')
                {
                    isip6 = false;
                    continue;
                }
                if (isip6)
                {
                    temphost.push_back(str[j]);
                }
                else
                {
                    if (str[j] == ':')
                    {
                        isport = true;
                        continue;
                    }
                    if (isport)
                    {
                        tempport.push_back(str[j]);
                    }
                    else
                    {
                        temphost.push_back(str[j]);
                    }
                }
            }
            temp["host"] = temphost;
            if (tempport.size() > 0)
            {
                temp["port"] = tempport;
            }
            i     = j;
            isurl = true;
        }
        else if (((i + 7) < str.size()) && str[i + 1] == 't' && str[i + 2] == 't' && str[i + 3] == 'p' && str[i + 4] == 's' &&
                 str[i + 5] == ':' && str[i + 6] == '/' && str[i + 7] == '/')
        {
            temp["scheme"] = "https";
            bool isip6     = false;
            bool isport    = false;
            std::string temphost;
            std::string tempport;
            unsigned int j = i + 8;
            for (; j < str.size(); j++)
            {
                if (str[j] == '/')
                {
                    break;
                }
                if (str[j] == '[')
                {
                    isip6 = true;
                    continue;
                }
                if (str[j] == ']')
                {
                    isip6 = false;
                    continue;
                }
                if (isip6)
                {
                    temphost.push_back(str[j]);
                }
                else
                {
                    if (str[j] == ':')
                    {
                        isport = true;
                        continue;
                    }
                    if (isport)
                    {
                        tempport.push_back(str[j]);
                    }
                    else
                    {
                        temphost.push_back(str[j]);
                    }
                }
            }
            temp["host"] = temphost;
            if (tempport.size() > 0)
            {
                temp["port"] = tempport;
            }
            i     = j;
            isurl = true;
        }
    }

    if (str[i] == '/')
    {
        temp["dirname"] = "/";
    }
    for (; i < str.size(); i++)
    {
        if (isurl)
        {
            if (str[i] == '#')
            {
                break;
            }
            if (str[i] == '?')
            {
                for (unsigned int j = i + 1; j < str.size(); j++)
                {
                    if (str[j] == '#')
                    {
                        break;
                    }
                    temp["querystring"].push_back(str[j]);
                }
                break;
            }
        }
        if (str[i] == '/')
        {
            if (temp["basename"].size() > 0)
            {
                if (temp["basename"].size() == 1 && temp["basename"][0] == '.')
                {
                }
                else if (temp["basename"].size() == 2 && temp["basename"] == "..")
                {

                    if (vpath.size() > 0)
                    {
                        vpath.pop_back();
                    }
                }
                else
                {
                    vpath.push_back(temp["basename"]);
                }
            }
            temp["basename"]  = "";
            temp["extension"] = "";
            temp["filename"]  = "";
            isext             = false;

            continue;
        }
        temp["basename"].push_back(str[i]);
        if (str[i] == '.')
        {
            if (isext)
            {
                temp["filename"].push_back('.');
                temp["filename"].append(temp["extension"]);
                temp["extension"] = "";
            }
            isext = true;
            continue;
        }

        if (isext)
        {
            temp["extension"].push_back(str[i]);
        }
        else
        {
            temp["filename"].push_back(str[i]);
        }
    }
    if (vpath.size() > 0)
    {
        for (unsigned int j = 0; j < vpath.size(); j++)
        {
            if (j > 0)
            {
                temp["dirname"].push_back('/');
            }
            temp["dirname"].append(vpath[j]);
        }
    }
    return temp;
}
std::string str_trim(std::string_view str)
{
    std::string temp;
    unsigned int tlen = str.size();
    for (; tlen > 0; tlen--)
    {
        unsigned int j = tlen - 1;
        if (str[j] == 0x20 || str[j] == 0x09 || str[j] == 0x0A || str[j] == 0x0D)
        {
            continue;
        }
        break;
    }
    unsigned int i = 0;
    for (; i < tlen; i++)
    {
        if (str[i] == 0x20 || str[i] == 0x09 || str[i] == 0x0A || str[i] == 0x0D)
        {
            continue;
        }
        break;
    }
    for (; i < tlen; i++)
    {
        temp.push_back(str[i]);
    }
    return temp;
}
std::string_view str_trim_view(std::string_view str)
{
    unsigned int tlen = str.size();
    for (; tlen > 0; tlen--)
    {
        unsigned int j = tlen - 1;
        if (str[j] == 0x20 || str[j] == 0x09 || str[j] == 0x0A || str[j] == 0x0D)
        {
            continue;
        }
        break;
    }
    unsigned int i = 0;
    for (; i < tlen; i++)
    {
        if (str[i] == 0x20 || str[i] == 0x09 || str[i] == 0x0A || str[i] == 0x0D)
        {
            continue;
        }
        break;
    }
    tlen = tlen - i;
    return str.substr(i, tlen);
}

std::string html_encode(std::string_view str)
{
    std::string temp;
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (str[i] == '&')
        {
            temp.append("&amp;");
            continue;
        }
        else if (str[i] == 0x22)
        {
            // double quote
            temp.append("&quot;");
            continue;
        }
        else if (str[i] == 0x27)
        {
            // single quote &#039; ie not support &apos;
            temp.append("&apos;");
            continue;
        }
        else if (str[i] == 0x3C)
        {
            //<
            temp.append("&lt;");
            continue;
        }
        else if (str[i] == 0x3E)
        {
            //>
            temp.append("&gt;");
            continue;
        }
        else
        {
            temp.push_back(str[i]);
        }
    }
    return temp;
}
long long str2int(std::string_view source)
{
    long long temp      = 0;
    unsigned int qi     = 0;
    unsigned int length = source.size();
    bool issub          = false;
    for (; qi < length; qi++)
    {
        if (source[qi] != 0x20)
        {
            break;
        }
    }
    if (qi < length && source[qi] == '-')
    {
        issub = true;
        qi++;
    }
    for (; qi < length; qi++)
    {
        if (source[qi] < 0x3A && source[qi] > 0x2F)
        {
            temp = temp * 10 + (source[qi] - 0x30);
        }
    }
    if (issub)
    {
        temp = ~temp + 1;
    }
    return temp;
}
long long str2int(const char *source, unsigned int str_length)
{
    long long temp  = 0;
    unsigned int qi = 0;
    bool issub      = false;
    for (; qi < str_length; qi++)
    {
        if (source[qi] != 0x20)
        {
            break;
        }
    }
    if (source[qi] == '-')
    {
        issub = true;
        qi++;
    }
    for (; qi < str_length; qi++)
    {
        if (source[qi] < 0x3A && source[qi] > 0x2F)
        {
            temp = temp * 10 + (source[qi] - 0x30);
        }
    }
    if (issub)
    {
        temp = ~temp + 1;
    }
    return temp;
}
std::string char2hex(const unsigned char *source, unsigned int str_length, unsigned char sp)
{
    static const unsigned char str[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string obj;
    unsigned int qi = 0;

    for (; qi < str_length; qi++)
    {
        unsigned char tmc = source[qi];
        obj.push_back(str[tmc >> 4 & 0x0F]);
        obj.push_back(str[tmc & 0x0F]);
        if (sp > 0)
        {
            if ((qi + 1) % sp == 0)
            {
                obj.push_back(0x20);
            }
        }
    }
    return obj;
}
std::string str2safepath(const char *source, unsigned int str_length)
{
    std::string temp;
    for (unsigned int i = 0; i < str_length; i++)
    {

        if ((source[i] > 0x2F && source[i] < 0x3A) || source[i] == '(' || source[i] == ')' || source[i] == '~' ||
            source[i] == '_' || source[i] == '-' || (source[i] > 0x40 && source[i] < 0x5B) ||
            (source[i] > 0x60 && source[i] < 0x7B))
        {
            temp.push_back(source[i]);
        }
    }
    return temp;
}
std::string str2safefile(const char *source, unsigned int str_length)
{
    std::string temp;
    for (unsigned int i = 0; i < str_length; i++)
    {

        if ((source[i] > 0x2F && source[i] < 0x3A) || source[i] == '.' || source[i] == '[' || source[i] == ']' ||
            source[i] == '(' || source[i] == ')' || source[i] == '~' || source[i] == '_' || source[i] == '-' ||
            (source[i] > 0x40 && source[i] < 0x5B) || (source[i] > 0x60 && source[i] < 0x7B))
        {
            temp.push_back(source[i]);
        }
    }
    return temp;
}
std::string str2safemethold(const char *source, unsigned int str_length)
{
    std::string temp;
    for (unsigned int i = 0; i < str_length; i++)
    {

        if ((source[i] > 0x2F && source[i] < 0x3A) || source[i] == '_' || (source[i] > 0x40 && source[i] < 0x5B) ||
            (source[i] > 0x60 && source[i] < 0x7B))
        {
            temp.push_back(source[i]);
        }
    }
    return temp;
}
std::string array_to_sql(const std::vector<std::string> a, char b, bool isquote)
{
    std::string temp;
    for (unsigned int j = 0; j < a.size(); j++)
    {
        if (j > 0)
        {
            temp.push_back(b);
        }
        if (isquote)
        {
            temp.append(a[j]);
        }
        else
        {
            temp.push_back('"');
            temp.append(str_addslash(a[j]));
            temp.push_back('"');
        }
    }
    return temp;
}
std::string str_addslash(const std::string &content)
{
    std::string temp;
    for (unsigned int i = 0; i < content.size(); i++)
    {
        if (content[i] == '\'')
        {
            temp.append("\\'");
            continue;
        }
        else if (content[i] == '"')
        {
            temp.append("\\\"");
            continue;
        }
        else if (content[i] == '\\')
        {
            temp.append("\\\\");
            continue;
        }
        temp.push_back(content[i]);
    }
    return temp;
}
std::string json_addslash(const std::string &content)
{
    std::string temp;
    for (unsigned int i = 0; i < content.size(); i++)
    {
        if (content[i] == '"')
        {
            temp.append("\\\"");
            continue;
        }
        else if (content[i] == '\\')
        {
            temp.append("\\\\");
            continue;
        }
        temp.push_back(content[i]);
    }
    return temp;
}

std::string strip_html(std::string_view content)
{
    std::string temp;
    std::string tempstr;
    unsigned int tag_count = 0;

    for (unsigned int i = 0; i < content.size(); i++)
    {
        if (content[i] == '<')
        {
            if (content[i + 1] > 0x20 && content[i + 1] < 0x7F)
            {
                i++;
                tempstr.clear();
                for (; i < content.size(); i++)
                {
                    if (content[i] == '>' || content[i] == ' ' || content[i] == '\t' || content[i] == 0x0D ||
                        content[i] == 0x0A)
                    {
                        break;
                    }
                    tempstr.push_back(content[i]);
                }

                if (str_casecmp(tempstr, "style"))
                {
                    for (; i < content.size(); i++)
                    {
                        if (content[i] == '>')
                        {
                            break;
                        }
                        if (content[i] == '"')
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '"' && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                        else if (content[i] == 0x27)
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == 0x27 && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                    }
                    for (; i < content.size(); i++)
                    {
                        if (content[i] == '<' && content[i + 1] == 0x2F)
                        {
                            i += 2;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '>' || content[i] == ' ' || content[i] == '\t' ||
                                    content[i] == 0x0D || content[i] == 0x0A)
                                {
                                    continue;
                                }
                                break;
                            }
                            tempstr.clear();
                            for (; i < content.size(); i++)
                            {
                                if ((content[i] > 0x40 && content[i] < 0x5B) ||
                                    (content[i] > 0x60 && content[i] < 0x7B))
                                {
                                    tempstr.push_back(content[i]);
                                    continue;
                                }
                                break;
                            }
                            if (str_casecmp(tempstr, "style"))
                            {
                                for (; i < content.size(); i++)
                                {
                                    if (content[i] == ' ' || content[i] == '\t')
                                    {
                                        continue;
                                    }
                                    break;
                                }
                                break;
                            }
                        }
                        if (content[i] == '"')
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '"' && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                        else if (content[i] == 0x27)
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == 0x27 && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                    }
                    continue;
                }
                else if (str_casecmp(tempstr, "script"))
                {
                    for (; i < content.size(); i++)
                    {
                        if (content[i] == '>')
                        {
                            i++;
                            break;
                        }
                        if (content[i] == '"')
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '"' && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                        else if (content[i] == 0x27)
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == 0x27 && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                    }

                    for (; i < content.size(); i++)
                    {
                        if (content[i] == '<' && content[i + 1] == 0x2F)
                        {
                            i += 2;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '>' || content[i] == ' ' || content[i] == '\t' ||
                                    content[i] == 0x0D || content[i] == 0x0A)
                                {
                                    continue;
                                }
                                break;
                            }

                            tempstr.clear();
                            for (; i < content.size(); i++)
                            {
                                if ((content[i] > 0x40 && content[i] < 0x5B) ||
                                    (content[i] > 0x60 && content[i] < 0x7B))
                                {
                                    tempstr.push_back(content[i]);
                                    continue;
                                }
                                break;
                            }

                            if (str_casecmp(tempstr, "script"))
                            {
                                for (; i < content.size(); i++)
                                {
                                    if (content[i] == ' ')
                                    {
                                        continue;
                                    }
                                    break;
                                }

                                break;
                            }
                        }

                        if (content[i] == '"')
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == '"' && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                        else if (content[i] == 0x27)
                        {
                            i++;
                            for (; i < content.size(); i++)
                            {
                                if (content[i] == 0x27 && content[i - 1] != 0x5C)
                                {
                                    break;
                                }
                            }
                            continue;
                        }
                    }
                    continue;
                }

                tag_count = 0;
                // nornal tag
                for (; i < content.size(); i++)
                {
                    if (content[i] == '>')
                    {
                        if (tag_count == 0)
                        {
                            break;
                        }
                        else
                        {
                            tag_count -= 1;
                            continue;
                        }
                    }
                    if (content[i] == '"')
                    {
                        i++;
                        for (; i < content.size(); i++)
                        {
                            if (content[i] == '"' && content[i - 1] != 0x5C)
                            {
                                break;
                            }
                        }
                        continue;
                    }
                    if (content[i] == 0x27)
                    {
                        i++;
                        for (; i < content.size(); i++)
                        {
                            if (content[i] == 0x27 && content[i - 1] != 0x5C)
                            {
                                break;
                            }
                        }
                        continue;
                    }
                    if (content[i] == '<')
                    {
                        // may be script tag in tag
                        tag_count += 1;
                        continue;
                    }
                }
                continue;
            }
            else
            {
                continue;
            }
        }
        temp.push_back(content[i]);
    }

    return temp;
}
std::string strip_annot(std::string_view content)
{
    std::string temp;
    for (unsigned int i = 0; i < content.size(); i++)
    {
        if (content[i] == '/' && ((i + 1) < content.size()) && content[i + 1] == '/')
        {

            i += 2;
            for (; i < content.size(); i++)
            {
                if (content[i] == 0x0A)
                {
                    if (i < content.size() && content[i] == 0x0D)
                    {
                        i++;
                    }
                    break;
                }
            }
            continue;
        }
        else if (content[i] == '/' && ((i + 1) < content.size()) && content[i + 1] == '*')
        {
            i += 2;
            for (; i < content.size(); i++)
            {
                if (content[i] == '*')
                {
                    if ((i + 1) < content.size() && content[i + 1] == '/')
                    {
                        i += 1;
                        break;
                    }
                }
            }
            continue;
        }
        else if (content[i] == '"')
        {
            temp.push_back(content[i]);
            i++;
            for (; i < content.size(); i++)
            {
                temp.push_back(content[i]);
                if (content[i] == '"' && content[i - 1] != 0x5C)
                {
                    break;
                }
            }
            continue;
        }
        else if (content[i] == 0x27)
        {
            temp.push_back(content[i]);
            i++;
            for (; i < content.size(); i++)
            {
                temp.push_back(content[i]);
                if (content[i] == 0x27 && content[i - 1] != 0x5C)
                {
                    break;
                }
            }
            continue;
        }

        temp.push_back(content[i]);
    }
    return temp;
}

void get_directory_all_file(std::map<unsigned long long, std::string> &listobj,
                            const std::string &file_path,
                            const std::string &url_path,
                            const std::string &extfile)
{
    std::filesystem::path tagetpath = file_path;
    std::string extname;
    std::string filename;
    std::string urlname = url_path;
    std::string temp_str;
    if (urlname.size() > 0 && urlname.back() != '/')
    {
        urlname.push_back('/');
    }
    if (std::filesystem::exists(tagetpath) && std::filesystem::is_directory(tagetpath))
    {
        for (const auto &entry : std::filesystem::directory_iterator(tagetpath))
        {
            auto filename = entry.path().filename().string();
            if (std::filesystem::is_regular_file(entry.status()))
            {
                extname = entry.path().extension().string();
                if (extfile.size() == 1 && extfile[0] == '*')
                {
                    unsigned long long time_last_edit = 0;
                    time_last_edit =
                        std::chrono::duration_cast<std::chrono::seconds>(entry.last_write_time().time_since_epoch())
                            .count();
                    temp_str                = urlname + filename;
                    listobj[time_last_edit] = temp_str;
                }
                else if (filename.size() > 0 && filename[0] != '.')
                {
                    if (extname.size() > 0 && extfile.find(extname) != std::string::npos)
                    {
                        unsigned long long time_last_edit = 0;
                        time_last_edit =
                            std::chrono::duration_cast<std::chrono::seconds>(entry.last_write_time().time_since_epoch())
                                .count();
                        temp_str                = urlname + filename;
                        listobj[time_last_edit] = temp_str;
                    }
                }
            }
            else if (std::filesystem::is_directory(entry.status()))
            {
                std::string tempurlpath  = urlname + filename + "/";
                std::string tempfilepath = file_path + filename + "/";
                get_directory_all_file(listobj, tempfilepath, tempurlpath, extfile);
            }
        }
    }
}

std::string get_safepath(std::string_view source)
{
    std::string temp;
    for (unsigned int i = 0; i < source.size(); i++)
    {
        char temc = source[i];
        if (temc >= 'A' && temc <= 'Z')
        {
            temp.push_back(temc + 32);
        }
        else if (temc >= 'a' && temc <= 'z')
        {
            temp.push_back(temc);
        }
        else if (temc >= '0' && temc <= '9')
        {
            temp.push_back(temc);
        }
        else if (temc == '-')
        {
            temp.push_back(temc);
        }
        else if (temc == '_')
        {
            temp.push_back(temc);
        }
        else if (temc == '.')
        {
            temp.push_back(temc);
        }
    }
    return temp;
}

std::string numstr_to_sql(const char *source, unsigned int str_length, char b)
{
    std::string tempt;
    std::string tempstr;

    for (unsigned int i = 0; i < str_length; i++)
    {
        if (source[i] == '-' || (source[i] > 0x2F && source[i] < 0x3A))
        {
            tempstr.push_back(source[i]);
        }
        else if (source[i] == b)
        {
            if (tempstr.size() > 0)
            {
                if (tempt.size() > 0)
                {
                    tempt.push_back(b);
                }
                tempt.append(tempstr);
            }
            tempstr.clear();
        }
    }
    if (tempstr.size() > 0)
    {
        if (tempt.size() > 0)
        {
            tempt.push_back(b);
        }
        tempt.append(tempstr);
    }
    return tempt;
}

double money_get_num(long long a)
{
    //may be define MONEY_PART 1000000
    return (double)((long double)a / CONST_MONEY_PART);
}
long long money_put_num(double a)
{
    return std::round(a * CONST_MONEY_PART);
}

long long num_get_money(long long a)
{
    //may be define MONEY_PART 1000000
    return std::round(a / CONST_MONEY_PART);
}
long long num_put_money(long long a)
{
    return a * CONST_MONEY_PART;
}

}// namespace http
