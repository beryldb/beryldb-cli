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
 
#include "emerald.h"

unsigned const char ascii_case_insensitive_map[256] = 
{
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
        10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
        20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
        30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
        40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
        50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
        60,  61,  62,  63,  64,  97,  98,  99,  100, 101,
        102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
        122, 91,  92,  93,  94,  95,  96,  97,  98,  99,
        100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
        130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
        140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
        150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
        170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
        180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
        190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
        200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
        210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
        220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
        230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
        250, 251, 252, 253, 254, 255,
};

unsigned const char brld_case_insensitive_map[256] = 
{
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
        10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
        20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
        30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
        40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
        50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
        60,  61,  62,  63,  64,  97,  98,  99,  100, 101,
        102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
        122, 123, 124, 125, 94,  95,  96,  97,  98,  99,
        100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
        130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
        140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
        150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
        160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
        170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
        180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
        190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
        200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
        210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
        220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
        230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
        240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
        250, 251, 252, 253, 254, 255,
};

bool engine::equals(const std::string& s1, const std::string& s2)
{
	const unsigned char* n1 = (const unsigned char*)s1.c_str();
	const unsigned char* n2 = (const unsigned char*)s2.c_str();

	for (; *n1 && *n2; n1++, n2++)
	{
		if (locale_case_insensitive_map[*n1] != locale_case_insensitive_map[*n2])
		{
			return false;
		}
	}
	
	return (locale_case_insensitive_map[*n1] == locale_case_insensitive_map[*n2]);
}

size_t engine::find(const std::string& haystack, const std::string& needle)
{
	if (needle.length() > haystack.length())
	{
		return std::string::npos;
	}
	
	size_t haystack_last = haystack.length() - needle.length();
	
	for (size_t hpos = 0; hpos <= haystack_last; ++hpos)
	{
		bool found = true;
	
		for (size_t npos = 0; npos < needle.length(); ++npos)
		{
			if (locale_case_insensitive_map[(unsigned char)needle[npos]] != locale_case_insensitive_map[(unsigned char)haystack[hpos + npos]])
			{
				found = false;
				break;
			}
		}
	
		if (found)
		{
			return hpos;
		}
	}
	
	return std::string::npos;
}

bool engine::insensitive_swo::operator()(const std::string& a, const std::string& b) const
{
	const unsigned char* charmap = locale_case_insensitive_map;
	std::string::size_type asize = a.size();
	std::string::size_type bsize = b.size();
	std::string::size_type maxsize = std::min(asize, bsize);

	for (std::string::size_type i = 0; i < maxsize; i++)
	{
		unsigned char A = charmap[(unsigned char)a[i]];
		unsigned char B = charmap[(unsigned char)b[i]];
		if (A > B)
			return false;
		else if (A < B)
			return true;
	}

	return (asize < bsize);
}

size_t engine::insensitive::operator()(const std::string &s) const
{
	size_t t = 0;
	
	for (std::string::const_iterator x = s.begin(); x != s.end(); ++x) 
	{
		t = 5 * t + locale_case_insensitive_map[(unsigned char)*x];
	
	}
	
	return t;
}

engine::token_node_stream::token_node_stream(const std::string& msg, size_t start, size_t end)
	: message(msg, start, end)
	, position(0)
{

}

bool engine::token_node_stream::get_middle(std::string& token)
{
	if (position >= message.length())
	{
		token.clear();
		return false;
	}
	
	size_t separator = message.find(' ', position);
	
	if (separator == std::string::npos)
	{
		token.assign(message, position, std::string::npos);
		position = message.length();
		return true;
	}

	token.assign(message, position, separator - position);
	position = message.find_first_not_of(' ', separator);
	return true;
}

bool engine::token_node_stream::get_trailing(std::string& token)
{
	if (position >= message.length())
	{
		token.clear();
		return false;
	}
	
	if (message[position] == ':')
	{
		token.assign(message, position + 1, std::string::npos);
		position = message.length();
		return true;
	}
	
	return get_middle(token);
}

engine::node_stream::node_stream(const std::string& source, char separator, bool allowempty)
	: tokens(source), sep(separator), pos(0), allow_empty(allowempty)
{

}

bool engine::node_stream::items_extract(std::string &token)
{
	if (this->stream_finish())
	{
		token.clear();
		return false;
	}

	if (!this->allow_empty)
	{
		this->pos = this->tokens.find_first_not_of(this->sep, this->pos);

		if (this->pos == std::string::npos)
		{
			this->pos = this->tokens.length() + 1;
			token.clear();
			return false;
		}
	}

	size_t p = this->tokens.find(this->sep, this->pos);
	
	if (p == std::string::npos)
	{
		p = this->tokens.length();
	}

	token.assign(tokens, this->pos, p - this->pos);
	this->pos = p + 1;

	return true;
}

const std::string engine::node_stream::get_remaining()
{
	return !this->stream_finish() ? this->tokens.substr(this->pos) : "";
}

bool engine::node_stream::stream_finish()
{
	return this->pos > this->tokens.length();
}

bool engine::node_stream::contains(const std::string& value)
{
	std::string token;

	while (items_extract(token))
	{
		if (value == token)
		{
			return true;
		}
	}

	return false;
}

