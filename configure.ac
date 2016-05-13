#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rename-tool], [1.99.1], [github@jdesch.de])
AM_INIT_AUTOMAKE([1.9 foreign])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([libintl.h stddef.h stdlib.h string.h strings.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero getcwd memmove memset re_comp regcomp strchr strrchr])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
