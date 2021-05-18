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

#include <algorithm>
#include <string.h>

namespace stdhelpers
{
	namespace vector
	{
		
		template <typename T>
		inline void swaperase(typename std::vector<T>& vect, const typename std::vector<T>::iterator& it)
		{
			*it = vect.back();
			vect.pop_back();
		}

		
		template <typename T>
		inline bool swaperase(typename std::vector<T>& vect, const T& val)
		{
			const typename std::vector<T>::iterator it = std::find(vect.begin(), vect.end(), val);
			if (it != vect.end())
			{
				swaperase(vect, it);
				return true;
			}
			return false;
		}
	}

	namespace string
	{
		
		inline const char* tocstr(const char* str)
		{
			return str;
		}

		
		inline const char* tocstr(const std::string& str)
		{
			return str.c_str();
		}

		
		template <typename S1, typename S2>
		inline bool equalsci(const S1& str1, const S2& str2)
		{
			return (!strcasecmp(tocstr(str1), tocstr(str2)));
		}

		
		template<typename Collection>
		inline std::string join(const Collection& sequence, char separator = ' ')
		{
			std::string joined;
			if (sequence.empty())
				return joined;

			for (typename Collection::const_iterator iter = sequence.begin(); iter != sequence.end(); ++iter)
				joined.append(convto_string(*iter)).push_back(separator);

			joined.erase(joined.end() - 1);
			return joined;
		}

		
		template<typename CharT, typename Traits, typename Alloc>
		inline bool replace(std::basic_string<CharT, Traits, Alloc>& str, const std::basic_string<CharT, Traits, Alloc>& target, const std::basic_string<CharT, Traits, Alloc>& replacement)
		{
			const typename std::basic_string<CharT, Traits, Alloc>::size_type p = str.find(target);
			if (p == std::basic_string<CharT, Traits, Alloc>::npos)
				return false;
			str.replace(p, target.size(), replacement);
			return true;
		}

		
		template<typename CharT, typename Traits, typename Alloc>
		inline void replace_all(std::basic_string<CharT, Traits, Alloc>& str, const std::basic_string<CharT, Traits, Alloc>& target, const std::basic_string<CharT, Traits, Alloc>& replacement)
		{
			if (target.empty())
				return;

			typename std::basic_string<CharT, Traits, Alloc>::size_type p = 0;
			while ((p = str.find(target, p)) != std::basic_string<CharT, Traits, Alloc>::npos)
			{
				str.replace(p, target.size(), replacement);
				p += replacement.size();
			}
		}
	}

	
	template <typename T>
	struct defaultdeleter
	{
		void operator()(T* o)
		{
			delete o;
		}
	};

	
	
	template <template<typename, typename> class Cont, typename T, typename Alloc>
	inline void delete_all(const Cont<T*, Alloc>& cont)
	{
		std::for_each(cont.begin(), cont.end(), defaultdeleter<T>());
	}

	
	template<typename T>
	void delete_zero(T*& pr)
	{
		T* p = pr;
		pr = NULL;
		delete p;
	}

	
	template <template<typename, typename> class Cont, typename T, typename Alloc>
	inline bool erase(Cont<T, Alloc>& cont, const T& val)
	{
		const typename Cont<T, Alloc>::iterator it = std::find(cont.begin(), cont.end(), val);
		if (it != cont.end())
		{
			cont.erase(it);
			return true;
		}
		return false;
	}

	
	template <template<typename, typename> class Cont, typename T, typename Alloc>
	inline bool isin(const Cont<T, Alloc>& cont, const T& val)
	{
		return (std::find(cont.begin(), cont.end(), val) != cont.end());
	}

	namespace string
	{
		
		template <char from, char to, char esc>
		inline void escape(const std::string& str, std::string& out)
		{
			for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
			{
				char c = *i;
				if (c == esc)
					out.append(2, esc);
				else
				{
					if (c == from)
					{
						out.push_back(esc);
						c = to;
					}
					out.push_back(c);
				}
			}
		}

		
		template <char from, char to>
		inline void escape(const std::string& str, std::string& out)
		{
			escape<from, to, '\\'>(str, out);
		}

		
		template<char from, char to, char esc>
		inline bool unescape(const std::string& str, std::string& out)
		{
			for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
			{
				char c = *i;
				if (c == '\\')
				{
					++i;
					if (i == str.end())
						return false;

					char nextc = *i;
					if (nextc == esc)
						c = esc;
					else if (nextc != to)
						return false;
					else
						c = from;
				}
				out.push_back(c);
			}
			return true;
		}

		
		template <char from, char to>
		inline bool unescape(const std::string& str, std::string& out)
		{
			return unescape<from, to, '\\'>(str, out);
		}
	}
}
