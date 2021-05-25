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

#include "nodes.h"

#define DONE 1
#define INFO 2
#define ERROR 3
#define WARNING 4

class Emerald;
class Configuration;
class Server;
class Handlers;
class Engine;
class config_rule;

#if defined _LIBCPP_VERSION || defined _WIN32 || __cplusplus >= 201103L
# define STR1 std
# include <array>
# include <functional>
# include <unordered_map>
# include <type_traits>
#else
# define STR1 std::tr1
# include <tr1/array>
# include <tr1/functional>
# include <tr1/unordered_map>
# include <tr1/type_traits>
#endif

typedef brld::flat_map<std::string, std::string, engine::insensitive_swo> file_config_items;

typedef std::multimap<std::string, reference<config_rule>, engine::insensitive_swo> ConfigMap;
typedef std::vector<std::string> file_cache;

typedef std::map<std::string, file_cache> cached_config;

typedef ConfigMap::const_iterator config_iterator;

typedef std::pair<config_iterator, config_iterator> MultiTag;


/* bprint is short for "beryl print", which prints out to terminal. */

#define bprint Kernel->Engine->printb

#if defined __clang__ || defined __GNUC__
# define BERYL_PRINTF(stringpos, firstpos) __attribute__((format(printf, stringpos, firstpos)))
#else
# define BERYL_PRINTF(stringpos, firstpos)
#endif

#define EmeraldStart int main(int argc, char** argv)

template <typename T> struct safecast
{
        T* operator->()
        {
                return static_cast<T*>(this);
        }
};

#define SCHEME(scheme, last, format) \
        do { \
        va_list _vaList; \
        va_start(_vaList, last); \
        scheme.assign(Daemon::Format(_vaList, format)); \
        va_end(_vaList); \
        } while (false);


#define slog Kernel->Logs->Log