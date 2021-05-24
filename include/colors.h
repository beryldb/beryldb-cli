
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

#pragma once

#include <ostream>
#include <stdio.h>
#include <unistd.h>

namespace engine
{
    namespace color
    {
		inline bool ColorsEnabled()
		{

#ifdef BERYL_DISABLE_COLORS

			return false;
#else
			return isatty(fileno(stdout));
#endif
		}

		inline std::ostream& green(std::ostream& stream)
		{
			if (!ColorsEnabled())
			{
				return stream;
			}

			return stream << "\033[1;32m";
		}

		inline std::ostream& red(std::ostream& stream)
		{
			if (!ColorsEnabled())
			{
				return stream;
			}

			return stream << "\033[1;31m";
		}

		inline std::ostream& white(std::ostream& stream)
		{
			if (!ColorsEnabled())
			{
				return stream;
			}

			return stream << "\033[0m";
		}

		inline std::ostream& whitebright(std::ostream& stream)
		{
			if (!ColorsEnabled())
			{
				return stream;
			}

			return stream << "\033[1m";
		}

                inline std::ostream& purple(std::ostream& stream)
                {
                        if (!ColorsEnabled())
                        {
                                return stream;
                        }

                        return stream << "\033[1;35m";
                }

                inline std::ostream& bold(std::ostream& stream)
                {
                        if (!ColorsEnabled())
                        {
                                return stream;
                        }

                        return stream << "\033[1m";
                }

                inline std::ostream& blue(std::ostream& stream)
                {
                        if (!ColorsEnabled())
                        {
                                return stream;
                        }

                        return stream << "\033[1;34m";
                }
                
		inline std::ostream& reset(std::ostream& stream)
		{
			if (!ColorsEnabled())
			{
				return stream;
			}

			return stream << "\033[0m";
		}
	}
}