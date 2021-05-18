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

#include <cfloat>
#include <climits>

#include "adaptables.h"

class config_rule : public refcountbase
{
  private:

        file_config_items items;

  public:

        const std::string tag;
        const std::string src_name;
        const int src_line;

        std::string as_string(const std::string& key, const std::string& def, const STR1::function<bool(const std::string&)>& validator);
        
        std::string as_string(const std::string& key, const std::string& def = "", size_t minlen = 0, size_t maxlen = UINT32_MAX);
        
        long as_int(const std::string& key, long def, long min = LONG_MIN, long max = LONG_MAX, bool force = false);
        
        unsigned long as_uint(const std::string& key, unsigned long def, unsigned long min = 0, unsigned long max = ULONG_MAX, bool force = false);
        
        double as_float(const std::string& key, double def, double min = DBL_MIN, double max = DBL_MAX);

        bool as_bool(const std::string& key, bool def = false);
        
        unsigned long get_duration(const std::string& key, unsigned long def, unsigned long min = 0, unsigned long max = ULONG_MAX);

        bool read_string(const std::string& key, std::string& value, bool allow_newline = false);

        std::string get_tag_location();

        inline const file_config_items& getItems() const 
        { 
                return items; 
        }

        static config_rule* create(const std::string& Tag, const std::string& file, int line, file_config_items*& Items);

 private:

        config_rule(const std::string& Tag, const std::string& file, int line);
};

struct UserArguments
{
        std::string host;
    
        std::string login;
        
        std::string port;
    
        std::string pass;
        
        bool do_tests;
        
        bool forcedebug;
                
        int argc;

        char** argv;
};

class Configuration
{
    public:
        
        Configuration();
        
        std::string host;
        
        std::string session;
        
        std::string login;
        
        std::string port;
        
        std::string pass;

        UserArguments usercmd;

        ConfigMap config_data;

        cached_config Files;

        config_rule* EmptyTag;

        std::stringstream errstr;
        
        std::string select;
        
        bool Ready;
        
        bool changeserv;
        
        bool notifyflags;
        
        bool clear;

        std::string customdisplay;
        
        config_rule* GetConf(const std::string& tag);

        MultiTag GetTags(const std::string& tag);

        class ServerPaths
        {
         public:
                
                std::string Config;

                std::string Runtime;

                std::string Log;
                
                ServerPaths(config_rule* tag);

                std::string PrependRuntime(const std::string& fn) const { return FileSystem::get_real_path(Runtime, fn); }

                std::string PrependConfig(const std::string& fn) const { return FileSystem::get_real_path(Config, fn); }
                
                std::string PrependLog(const std::string& fn) const { return FileSystem::get_real_path(Log, fn); }
                
        };

        ServerPaths Paths;

        bool RawLog;
        
        void Load();
        
        void Fill();
        
        void SetAll();
        
        void CheckCMD();
};

