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
 
#include "emerald.h"

static bool MatchInternal(const unsigned char* str, const unsigned char* mask, unsigned const char* map)
{
	unsigned char* cp = NULL;
	unsigned char* mp = NULL;
	unsigned char* string = (unsigned char*)str;
	unsigned char* wildcard = (unsigned char*)mask;

	while ((*string) && (*wildcard != '*'))
	{
		if ((map[*wildcard] != map[*string]) && (*wildcard != '?'))
		{
			return 0;
		}

		wildcard++;
		string++;
	}

	while (*string)
	{
		if (*wildcard == '*')
		{
			if (!*++wildcard)
			{
				return 1;
			}

			mp = wildcard;
			cp = string+1;
		}
		else
			if ((map[*wildcard] == map[*string]) || (*wildcard == '?'))
			{
				wildcard++;
				string++;
			}
			else
			{
				wildcard = mp;
				string = cp++;
			}

	}

	while (*wildcard == '*')
	{
		wildcard++;
	}

	return !*wildcard;
}

bool Daemon::Match(const std::string& str, const std::string& mask, unsigned const char* map)
{
	if (!map)
	{
		map = locale_case_insensitive_map;
	}

	return MatchInternal((const unsigned char*)str.c_str(), (const unsigned char*)mask.c_str(), map);
}

bool Daemon::Match(const char* str, const char* mask, unsigned const char* map)
{
	if (!map)
	{
		map = locale_case_insensitive_map;
	}

	return MatchInternal((const unsigned char*)str, (const unsigned char*)mask, map);
}


