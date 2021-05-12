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

#include <iostream>
#if defined _LIBCPP_VERSION || __cplusplus >= 201402L
# include <array>
# include <type_traits>
# include <unordered_map>
#else
# include <tr1/array>
# include <tr1/type_traits>
# include <tr1/unordered_map>
#endif

#if defined __llvm__ && !defined __clang__ && __GNUC__ == 4 && __GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ == 1
# error "LLVM-GCC 4.2.1 has broken visibility support."
#endif

int main() 
{
	std::cout << "Hello BerylDB!" << std::endl;
	return 0;
}
