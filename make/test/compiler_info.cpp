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

#include <iostream>

#if defined __INTEL_COMPILER // Also defines __clang__ and __GNUC__
# define BERYLDB_COMPILER_NAME "Intel"
# define BERYLDB_COMPILER_VERSION (__INTEL_COMPILER / 100) << '.' << (__INTEL_COMPILER % 100)
#elif defined __clang__ // Also defines __GNUC__
# if defined __apple_build_version__
#  define BERYLDB_COMPILER_NAME "AppleClang"
# else
#  define BERYLDB_COMPILER_NAME "Clang"
# endif
# define BERYLDB_COMPILER_VERSION __clang_major__ << '.' << __clang_minor__
#elif defined __GNUC__
# define BERYLDB_COMPILER_NAME "GCC"
# define BERYLDB_COMPILER_VERSION __GNUC__ << '.' << __GNUC_MINOR__
#endif

int main() 
{
	std::cout << "NAME " << BERYLDB_COMPILER_NAME << std::endl
	<< "VERSION " << BERYLDB_COMPILER_VERSION << std::endl;
	return 0;
}
