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

/* Predefined exit codes */

enum ExitCodes
{
        EXIT_CODE_OK = 0,          
        EXIT_CODE_CORE = 1,                     
        EXIT_CODE_CONFIG = 2,                   
        EXIT_CODE_LOG = 3,                      
        EXIT_CODE_ARGV = 4,                     
        EXIT_CODE_SOCKETSTREAM = 5,             
        EXIT_CODE_ROOT = 6,                     
        EXIT_CODE_SIGTERM = 7,         
        EXIT_CODE_SIGINT = 8                   
};

extern const char * ExitMap[];
