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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <memory>
#include <vector>

#include "config.h"
#include "constants.h"
#include "base.h"
#include "filehandler.h"
#include "stdhelpers.h"
#include "flat_map.h"
#include "adaptables.h"
#include "configprocess.h"
#include "colors.h"
#include "converter.h"
#include "protocols.h"
#include "server.h"
#include "handlers.h"
#include "nodes.h"
#include "engine.h"
#include "logs.h"

extern std::unique_ptr<Emerald> Kernel;

class Emerald
{
    friend class Daemon;
    friend class Server;
    friend class Configuration;
    friend class Handlers;
    
    private:
      
        /* Current time, as updated in the mainloop. */

        struct timespec TIME;
        
        /* Startup time */

        time_t startup; 
        
        /* Command line parser. */
        
        void CommandLine();

        /* My assigned (by the server) instance id. */
        
        std::string myself;
        
        /* Keep track of your flags. */
        
        std::string myflags;
        
        /* Real name of this server. */

        std::string realserver;
        
        /* Configuration file. */
        
        std::string ConfigFile;

   public:

        /* Server version. */
        
        std::string server_version;
        
        /* Server time */
        
        time_t server_time;
        
        /* Connected boolean. */
        
        bool Connected;
        
        /* > data to display. */

        std::string displayserver;

        /* Selected database */

        std::string select;

        /* Indicates whether we should display current select. */

        bool display_select;
        
        /* Server connection. */
        
        Server Link;

        /* Handles functions from server's protocol. */
        
        Handlers Handler;
        
        /* Internal functions. */
        
        Daemon Engine;

        /* Handles Emerald configuration files. */

        std::unique_ptr<Configuration> Config;

        /* Refreshes TIME */
        
        void Refresh();

        /*
         * Emerald's main. This function will initialize Emerald
         * and will read the config file.
         * 
         * @parameters:
         *
         *         · User commandline arguments.
         * 
         * @return:
         *
         *         · 1: Emerald is exiting.
         */
        
        Emerald(int argc, char** argv);

        /* Log Handler. */
        
	LogHandler Logs;

        /* Returns current time. */
        
        inline time_t Now() 
        { 
                return this->TIME.tv_sec; 
        }
        
        /* Returns current time, as expressed in microseconds. */
        
        inline long TimeStamp() 
        { 
                return this->TIME.tv_nsec; 
        }

        /* Returns startup time. */

        time_t GetStartup()
        {
                return this->startup;
        }
        
        /* Returns full path to configuration file. */
        
        const std::string GetConfigFile()
        {
                return  this->ConfigFile;
        }

        /* 
         * Exits Emerald.
         * 
         * @parameters:
         *
         *         · status: Exiting code.
         *         · skip: whether to skip exiting functions, such as history removal.
         *         · exitmsg: Exit message. 
         */            
         
        void Exit(int code = 0, bool skip = false, const std::string& exitmsg = "");

};

EmeraldStart;