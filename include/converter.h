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

#include <bitset>
#include <sstream>

inline std::string to_string(const std::string& data)
{
         std::stringstream sstream(data);
         std::string output;
         std::bitset<8> bits;

         while (sstream >> bits)
         {
              char c = char(bits.to_ulong());
              output += c;
         }

         return output;
}

/* Converts a string to binary. */

inline std::string to_bin(const std::string& data)
{
        std::string result;

        for (std::size_t i = 0; i < data.size(); ++i)
        {
              result.append(std::bitset<8>(data[i]).to_string());  
        }

        return result;
}

/* Checks whether a string has quotes (start and end) */

inline bool is_correct(const std::string& str)
{
	if (!str.length())
	{
		return false;
	}
	
	if (str.at(0) == '"' && str.back() == '"')
	{
		return true;
	}
	
	return false;
}

/* Checks if a number is a number or not. */

inline bool is_number(const std::string& str, bool allowdots = false)
{
    unsigned int counter = 0;

    for (char const &c : str) 
    {
        if (counter == 0 && (c == '-' || c == '+'))
        {
            continue;
        }

        if (allowdots && (c == '.' || c == '+' || c == 'e'))
        {
             counter++;
             continue;
        }
        
        if (std::isdigit(c) == 0) 
        {
             return false;
        }
        
        counter++;
    }
    
    return true;
}


/* Removes quote marks from a string. */

inline std::string stripe(const std::string& str)
{
        if (is_number(str, true))
        {
                return str;
        }
        
        if (str.length() == 1)
        {
                return str;
        }
        
        if (str.length() < 1)
        {
                return "";
        }
        
        std::string aux = str; 
        aux.erase(0, 1);
        aux.pop_back();
        
        return aux;
}

inline bool is_positive_number(const std::string& str)
{
         for (char const &c : str) 
         {
               if (std::isdigit(c) == 0) 
               {
                   return false;
               }
        }
 
        return true;
}

template<typename T> inline std::string conv_numeric(const T& in)
{
	if (in == 0)
	{
		return "0";
	}
	
	T quotient = in;
	std::string out;
	
	while (quotient)
	{
		out += "0123456789"[std::abs((long)quotient % 10)];
		quotient /= 10;
	}
	
	if (in < 0)
	{
		out += '-';
	}
	
	std::reverse(out.begin(), out.end());
	return out;
}

/* Converts and into to string. */

inline std::string convto_string(const int input)
{
	return conv_numeric(input);
}

/* Converts a long to a string. */

inline std::string convto_string(const long input)
{
	return conv_numeric(input);
}

/* Converts char to string. */

inline std::string convto_string(const char* input)
{
	return input;
}

/* Converts bool to string. */

inline std::string convto_string(const bool input)
{
	return (input ? "1" : "0");
}

inline std::string convto_string(char input)
{
	return std::string(1, input);
}

inline const std::string& convto_string(const std::string& input)
{
	return input;
}

template <class T> inline std::string convto_string(const T& input)
{
	std::stringstream tmp;

	if (!(tmp << input))
	{
		return std::string();
	}

	return tmp.str();
}

template<typename T> inline T convto_num(const std::string& str)
{
	T returning;
	std::istringstream tmp(str);
	
	if (!(tmp >> returning))
	{
		return 0;
	}
	
	return returning;
}

template<> inline char convto_num<char>(const std::string& str)
{
	int16_t value = convto_num<int16_t>(str);
	return value >= INT8_MIN && value <= INT8_MAX ? value : 0;
}

template<> inline unsigned char convto_num<unsigned char>(const std::string& str)
{
	uint16_t value = convto_num<uint16_t>(str);
	return value <= UINT8_MAX ? value : 0;
}


