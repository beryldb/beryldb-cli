/*
 * BerylDB - A modular database.
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

/*
 * server.cpp is largely based in Michael Czigler's IRC client: kirc.
 *
 * Many credits goes to the kirc team (Copyright 2021 - MIT license),
 * Michael and his great work. 
 *
 * kirc is based on linenoise (BSD-2), so credits goes to the linenoise team as well.
 */

#pragma once

#include <termios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "typedefs.h"

class Server : public safecast<Server>
{
    private:
    
        /* Current state */
        
        struct State cstate;

        struct pollfd fds[2];
    
        void Flush();
    
    public:
        
        /* Constructor. */
        
        Server();
        
        std::vector<std::string> buffer;
        
        /* Writes history file. */
        
        int HistoryWrite();
        
        /* Resets cache */
        
        void ResetCache();
        
        /* Inits Emerald's loop. */

        int Initialize();
        
        /* Raw socket write to remote server. */
        
        static void Write(char *fmt, ...);

        /* 
         * This function is called before the 
         * user sends a new command to the server. 
         */
        
        bool CheckCmd(const std::vector<std::string>& CommandList);
    
        /* Closes socket. */
        
        void Shutdown();        
        
        void PrepareExit();
        
        /* Clears screen */
        
        static void ClearScreen(void);        
        
        /* Returns history counter */
        
        unsigned int CountHistory();
        
};