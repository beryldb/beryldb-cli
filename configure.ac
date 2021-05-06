AC_PREREQ([2.69])
AC_INIT([algatorc], [0.1], [cferry@beryldb.com])
AC_CONFIG_SRCDIR([emerald/src/emerald.cpp])

LT_INIT
AM_INIT_AUTOMAKE

# Checks for programs.

AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([gettimeofday memset mkdir])

LIBS=-ldl

AC_CONFIG_FILES([Makefile])
AC_OUTPUT