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

#include <map>

class Handlers
{
    public:

        /* Constructor. */
        
        Handlers();

        static void Test();

        /* Called when BRLD_NEW_USE is triggered */
        
        void OnMyUse(std::vector<std::string>& cmd);

        /* Called when an user flags' change. */
        
        void OnYourFlags(std::vector<std::string>& cmd);

        /* Called as an user joins a channel. */
        
        void OnJoin(std::vector<std::string>& cmd);
        
        /* Called upon  a new publish msg */

        void OnPublish(std::vector<std::string>& cmd, std::string& original);

        /* Called as an user leaves a channel. */
        
        void OnPart(std::vector<std::string>& cmd);

        /* Called upon user is connected. This function sets servers' name. */
        
        void OnConnected(std::vector<std::string>& cmd);

        void MyInfo(std::vector<std::string>& cmd);

        /* Called as an user quits. */
        
        void OnQuit(std::vector<std::string>& cmd);

        /* Called when uptime is called. */
        
        std::string OnUptime(std::string& original);

         void OnSlist(std::vector<std::string>& cmd, std::string& original);

        static void Local(std::string& buffer);

        static void OnError(std::vector<std::string>& cmd, std::string& original);
        
        std::string OnNotFnd(std::vector<std::string>& cmd);

         void OnWrongPass();
};

