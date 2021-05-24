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

class Utils
{
    public:

    /* Obtains stdout data from terminal. */

    static std::string GetStdoutFromCommand(std::string cmd);

    /* Generates a random string. */

    static std::string GenRandom(const int len);

};