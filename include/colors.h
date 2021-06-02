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

#include <ostream>
#include <stdio.h>

#ifdef _WIN32
# include <iostream>
# include <io.h>
# define isatty(x) _isatty((x))
# define fileno(x) _fileno((x))
extern WindowsStream StandardError;
extern WindowsStream StandardOutput;
#else
# include <unistd.h>
#endif

namespace
{
	inline bool ColorsEnabled()
	{
#ifdef BERYL_DISABLE_COLORS
		return false;
#else
		return isatty(fileno(stdout));
#endif
	}

#ifdef _WIN32
	inline WindowsStream& GetStreamHandle(std::ostream& os)
	{
		if (os.rdbuf() == std::cerr.rdbuf())
		{
			return StandardError;
		}

		if (os.rdbuf() == std::cout.rdbuf())
		{
			return StandardOutput;
		}

		throw std::invalid_argument("Error");
	}
#endif
}

#ifdef _WIN32

#include <windows.h>

inline std::ostream& green(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY | ws.BackgroundColor);
	}
	
	return stream;
}

inline std::ostream& blue(std::ostream& stream)
{
        if (ColorsEnabled())
        {
                const WindowsStream& ws = GetStreamHandle(stream);
                SetConsoleTextAttribute(ws.Handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY | ws.BackgroundColor);
        }
        
        return stream;
}

inline std::ostream& red(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, FOREGROUND_RED | FOREGROUND_INTENSITY | ws.BackgroundColor);
	}
	
	return stream;
}

inline std::ostream& white(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | ws.BackgroundColor);
	}
	
	return stream;
}

inline std::ostream& white_bright(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | ws.BackgroundColor);
	}
	
	return stream;
}

inline std::ostream& bold(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, FOREGROUND_INTENSITY | ws.BackgroundColor);
	}
	
	return stream;
}

inline std::ostream& reset(std::ostream& stream)
{
	if (ColorsEnabled())
	{
		const WindowsStream& ws = GetStreamHandle(stream);
		SetConsoleTextAttribute(ws.Handle, ws.ForegroundColor);
	}
	
	return stream;
}

#else

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

inline std::ostream& white_bright(std::ostream& stream)
{
	if (!ColorsEnabled())
	{
		return stream;
	}
	
	return stream << "\033[1m";
}

inline std::ostream& bold(std::ostream& stream)
{
	if (!ColorsEnabled())
	{
		return stream;
	}
	
	return stream << "\033[1m";
}

inline std::ostream& reset(std::ostream& stream)
{
	if (!ColorsEnabled())
	{
		return stream;
	}
	
	return stream << "\033[0m";
}

inline std::ostream& blue(std::ostream& stream)
{
        if (!ColorsEnabled())
        {
                                return stream;
        }

        return stream << "\033[1;34m";
}

#endif
