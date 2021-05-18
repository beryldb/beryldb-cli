/*
 * Emerald - A POSIX client for BerylDB.
 * http://www.beryldb.com
 *
 * Copyright (C) 2021 - Carlos F. Ferry <cferry@beryldb.com>
 * 
 * This file is part of BerylDB. BerylDB is free software: you can
 * redistribute it and/or modify it under the terms of the BSD License
 * version 3.
 *
 * More information about our licensing can be found at https://docs.beryl.dev
 */
/// $CompilerFlags: -Ietc_directory("bcrypt")

#include <crypt_blowfish.c>

#include "emerald.h"
#include "engine.h"
#include "exit.h"

unsigned const char *locale_case_insensitive_map = brld_case_insensitive_map;

Daemon::Daemon() : ValidLogin(&LoginValidator), GenRandom(&DefaultGenRandom)
{

}

static const unsigned int num_duration[] =
{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 86400, 0, 0, 0, 3600, 0, 0, 0, 0, 60, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 604800, 0, 31557600, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 86400, 0, 0, 0, 3600, 0, 0, 0, 0, 60, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 604800, 0, 31557600, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

std::string Daemon::Format(va_list& vaList, const char* formatString)
{
        static std::vector<char> formatBuffer(1024);

        while (true)
        {
                va_list dst;
                va_copy(dst, vaList);

                int vsnret = vsnprintf(&formatBuffer[0], formatBuffer.size(), formatString, dst);
                va_end(dst);

                if (vsnret > 0 && static_cast<unsigned>(vsnret) < formatBuffer.size())
                {
                        break;
                }

                formatBuffer.resize(formatBuffer.size() * 2);
        }

        return std::string(&formatBuffer[0]);
}

std::string Daemon::Format(const char* formatString, ...)
{
        std::string ret;
        SCHEME(ret, formatString, formatString);
        return ret;
}


void Daemon::sprint(const int type, const char *fmt, ...)
{
       std::string buff;
       SCHEME(buff, fmt, fmt);
       Daemon::sprint(type, buff);
}

void Daemon::sprint(const int type, const std::string& buff)
{
    if (!Kernel->display_select)
    {
        std::cout << Daemon::Format("%s> %s", Kernel->displayserver.c_str(), buff.c_str()) << std::endl;
    }
    else
    {
        std::cout << Daemon::Format("%s[%s]> %s", Kernel->displayserver.c_str(), Kernel->select.c_str(), buff.c_str()) << std::endl;
    }
    
    if (type == DTYPE_R)
    {
         printf("\x1b[0m\r");
    }
    else if (type == DTYPE_RN)
    {
        printf("\x1b[0m\r\n");
    }
}

void Daemon::csprint(const int type, const char *fmt, ...)
{
       std::string buff;
       SCHEME(buff, fmt, fmt);
       Daemon::csprint(type, buff);
}

void Daemon::csprint(const int type, const std::string& buff)
{
    std::cout << Daemon::Format("%s", buff.c_str()) << std::endl;
    
    if (type == DTYPE_R)
    {
         printf("\x1b[0m\r");
    }
    else if (type == DTYPE_RN)
    {
        printf("\x1b[0m\r\n");
    }

}

void Daemon::printb(const int type, const char *fmt, ...)
{
       std::string buff;
       SCHEME(buff, fmt, fmt);
       this->printb(type, buff);
}

void Daemon::printb(const int type, const std::string& buff)
{
       switch (type)
       {
            case DONE:
                   
                    std::cout << "[ " << engine::color::green << "OK" << engine::color::reset << " ] " << buff << engine::color::reset << "\r\t" << std::endl;
                    break;

            case INFO:
                   
                    std::cout << "[ " << engine::color::blue << "INFO" << engine::color::reset << " ] " << buff  << engine::color::reset << "\r\t" << std::endl;
                    break;

            case ERROR:
                   
                    std::cout << "[ " << engine::color::red << "ERROR" << engine::color::reset << " ] " << buff << engine::color::reset << "\r\t" << std::endl;
                    break;
                    
            case WARNING:

                    std::cout << "[ " << engine::color::purple << "WARNING" << engine::color::reset << " ] " << buff << engine::color::reset << std::endl;
                    break;
                    

            default:
                   
                    std::cout << buff << std::endl;
       }
}

void Daemon::CheckRoot()
{
                if (getegid() != 0 && geteuid() != 0)
                {
                        return;
                }

                bprint(INFO, "You should never run Emerald as root.");
                Kernel->Exit(EXIT_CODE_ROOT);

}

bool Daemon::Duration(const std::string& str, unsigned long& duration)
{
        unsigned long total = 0;
        unsigned long subtotal = 0;

        for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
        {
                if ((*i >= '0') && (*i <= '9'))
                {
                        subtotal = (subtotal * 10) + (*i - '0');
                }
                else
                {
                        unsigned int multiplier = num_duration[static_cast<unsigned char>(*i)];

                        if (multiplier == 0)
                        {
                                return false;
                        }

                        total += subtotal * multiplier;

                        subtotal = 0;
                }
        }

        duration = total + subtotal;
        return true;
}

unsigned long Daemon::Duration(const std::string& str)
{
        unsigned long out = 0;
        Daemon::Duration(str, out);
        return out;
}


bool Daemon::ConfigFileExists(std::string& path)
{
        if (FileSystem::Exists(path))
        {
                return true;
        }

        return false;
}

std::string Daemon::Welcome(std::string& msg)
{
    const std::string& casted = msg;
    return Daemon::Welcome(casted);
}

std::string Daemon::Welcome(const std::string& msg)
{
        std::stringstream welcome(static_cast<std::stringstream&&>(std::stringstream() << engine::color::bold << msg << engine::color::reset));
        return welcome.str();
}

bool Daemon::LoginValidator(const std::string& login)
{
        /* Basic length validators. */
        
        if (login.empty() || login.length() < 3 || login.length() > 15)
        {
                return false;
        }

        for (std::string::const_iterator i = login.begin(); i != login.end(); ++i)
        {
                if ((*i >= 'A') && (*i <= '}'))
                {
                        continue;
                }

                if ((((*i >= '0') && (*i <= '9')) || (*i == '-')) && (i != login.begin()))
                {
                        continue;
                }

                return false;
        }

        return true;
}

unsigned long Daemon::generate_random_int(unsigned long max)
{
        unsigned long rv;
        GenRandom((char*)&rv, sizeof(rv));
        return rv % max;
}


std::string Daemon::generate_random_str(unsigned int length, bool printable)
{
        std::vector<char> str(length);
        GenRandom(&str[0], length);

        if (printable)
        {
                for (size_t i = 0; i < length; i++)
                {
                        str[i] = 0x3F + (str[i] & 0x3F);
                }
        }
                
        return std::string(&str[0], str.size());
}

void Daemon::DefaultGenRandom(char* output, size_t max)
{
        for (unsigned int i = 0; i < max; ++i)
        {
                output[i] = random();
        }
}

std::string BcryptServer::MakeSalt()
{
     char entropy[16];
                
     for (unsigned int i = 0; i < sizeof(entropy); ++i)
     {
           entropy[i] = Kernel->Engine->generate_random_int(0xFF);
     }

     char salt[32];
                
     if (!_crypt_gensalt_blowfish_rn("$2a$", DEFAULT_BROUNDS, entropy, sizeof(entropy), salt, sizeof(salt)))
     {
           return "";
     }

     return salt;
}

std::string BcryptServer::Generate(const std::string& data, const std::string& salt)
{
        char hash[64];
        _crypt_blowfish_rn(data.c_str(), salt.c_str(), hash, sizeof(hash));
        return hash;
}

std::string BcryptServer::MakePassword(const std::string& data)
{
        return BcryptServer::Generate(data, MakeSalt());
}

bool Daemon::TimingSafeCompare(const std::string& one, const std::string& two)
{
        if (one.length() != two.length())
        {
                return false;
        }

        unsigned int diff = 0;
        
        for (std::string::const_iterator i = one.begin(), j = two.begin(); i != one.end(); ++i, ++j)
        {
                unsigned char a = static_cast<unsigned char>(*i);
                unsigned char b = static_cast<unsigned char>(*j);
                diff |= a ^ b;
        }

        return (diff == 0);
}


std::string Daemon::HumanEpochTime(time_t curtime, const char* format, bool utc)
{
        struct tm* timeinfo = utc ? gmtime(&curtime) : localtime(&curtime);

        if (!timeinfo)
        {
                curtime = 0;
                timeinfo = localtime(&curtime);
        }

        if (timeinfo->tm_year + 1900 > 9999)
        {
                timeinfo->tm_year = 9999 - 1900;
        }
        else if (timeinfo->tm_year + 1900 < 1000)
        {
                timeinfo->tm_year = 0;
        }

        if (!format)
        {
                format = "%a %b %d %Y %H:%M:%S";
        }

        char buffer[512];

        if (!strftime(buffer, sizeof(buffer), format, timeinfo))
        {
                buffer[0] = '\0';
        }

        return buffer;
}
