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

#pragma once

class Methods
{
    public:
        
        Methods();
        
        /* Quits server. */
        
        static void Quit();

        static void Future(const unsigned int seconds, const std::string& key, const std::string& value);
        
        static void Command(const std::string& cmd);

        static void Publish(const std::string& dest, const std::string& text);

        static void Join(const std::string& chan);

        static void Part(const std::string& chan);

        static void LogIn(const std::string& session, const std::string& login, const std::string& pass);

        static void use(const std::string& value);

        static void rkey();

        static void Set(const std::string& key, const std::string& value);

        static void HSet(const std::string& key, const std::string& hesh, const std::string& value);

        static void HGet(const std::string& key, const std::string& hesh);
        
        static void Get(const std::string& key);

        static void LPush(const std::string& key, const std::string& value);

        static void LGet(const std::string& key);

        static void Find(const std::string& key);
        
};