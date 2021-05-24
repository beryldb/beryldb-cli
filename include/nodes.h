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

#include <cstring>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>

#include "emerald.h"
#include "converter.h"


extern unsigned const char *locale_case_insensitive_map;

extern unsigned const char brld_case_insensitive_map[256];

extern unsigned const char ascii_case_insensitive_map[256];

namespace engine
{
	bool equals(const std::string& s1, const std::string& s2);

	size_t find(const std::string& haystack, const std::string& needle);

	struct str_hash_comparison
	{
		bool operator()(const std::string& s1, const std::string& s2) const
		{
			return equals(s1, s2);
		}
	};

	struct insensitive
	{
		size_t operator()(const std::string &s) const;
	};

	struct insensitive_swo
	{
		bool operator()(const std::string& a, const std::string& b) const;
	};

	class node_stream
	{
	 protected:
		
		std::string tokens;
		
		char sep;
		
		size_t pos;
		
		bool allow_empty;
	 public:
		
		unsigned int count()
		{
				return tokens.size();
		}
		
		node_stream(const std::string &source, char separator, bool allowempty = false);

		
		bool items_extract(std::string& token);

		
		template<typename Numeric>
		bool GetNumericToken(Numeric& token)
		{
			std::string str;
			
			if (!items_extract(str))
			{
				return false;
			}

			token = convto_num<Numeric>(str);
			return true;
		}

		
		const std::string get_remaining();

		
		bool stream_finish();

		
		bool contains(const std::string& value);
	};

	
	class comma_node_stream : public node_stream
	{
	 public:
		
		comma_node_stream(const std::string &source, bool allowempty = false) : node_stream(source, ',', allowempty)
		{
		
		}
	};


        class newline_node_stream : public node_stream
        {
         public:

                newline_node_stream(const std::string &source, bool allowempty = false) : node_stream(source, '\n', allowempty)
                {
                
                }
        };
	
	class space_node_stream : public node_stream
	{
	 public:
		
		space_node_stream(const std::string &source, bool allowempty = false) : node_stream(source, ' ', allowempty)
		{

		}
	};

	class token_node_stream
	{
	private:
		
		std::string message;

		
		size_t position;

	 public:
		
		token_node_stream(const std::string& msg, size_t start = 0, size_t end = std::string::npos);

		
		std::string& get_message() { return message; }

		
		bool get_middle(std::string& token);

		
		bool get_trailing(std::string& token);
	};

}
