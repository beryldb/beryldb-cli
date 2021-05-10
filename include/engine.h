/*
 * Emerald - A POSIX client for BerylDB.
 * http://www.beryldb.com
 *
 * Copyright (C) 2015-2021 Carlos F. Ferry <cferry@beryldb.com>
 * 
 * This file is part of BerylDB. BerylDB is free software: you can
 * redistribute it and/or modify it under the terms of the BSD License
 * version 3.
 *
 * More information about our licensing can be found at https://docs.beryl.dev
 */

#pragma once

#include <iostream>

enum dtype
{
    DTYPE_R = 1,
    DTYPE_RN = 2,
    DTYPE_NONE = 3
};
    
class Daemon : public safecast<Daemon>
{
    public:
    
        /* Constructor. */
        
        Daemon();
        
        
        void printb(const int type, const std::string& buff);
        
        void printb(const int type, const char *fmt, ...) BERYL_PRINTF(3, 4);        

        static std::string Format(const char* formatString, ...) BERYL_PRINTF(1, 2);

        static std::string Format(va_list& vaList, const char* formatString) BERYL_PRINTF(2, 0);

        static void CheckRoot();

        static void csprint(const int type, const char *fmt, ...);

        static void csprint(const int type, const std::string& buff);
        
        static void sprint(const int type, const char *fmt, ...);

        static void sprint(const int type, const std::string& buff);

        static unsigned long Duration(const std::string& str);

        static bool Duration(const std::string& str, unsigned long& duration);
        
        static bool Match(const std::string& str, const std::string& mask, unsigned const char* map = NULL);
        static bool Match(const char* str, const char* mask, unsigned const char* map = NULL);
        
        static bool ConfigFileExists(std::string& path);
        
        static std::string Welcome(const std::string& msg);
        static std::string Welcome(std::string& msg);

        static bool LoginValidator(const std::string& login);        
        
        STR1::function<bool(const std::string&)> ValidLogin;

        static bool TimingSafeCompare(const std::string& one, const std::string& two);

        std::string generate_random_str(unsigned int length, bool printable = true);

        unsigned long generate_random_int(unsigned long max);
        
        static void DefaultGenRandom(char* output, size_t max);
        
        STR1::function<void(char*, size_t)> GenRandom;
};

class BcryptServer 
{
    public:
    
        static std::string MakeSalt();

        static std::string Generate(const std::string& data, const std::string& salt);
        
        static std::string MakePassword(const std::string& data);
};