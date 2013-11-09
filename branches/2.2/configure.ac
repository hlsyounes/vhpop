# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 2003 Carnegie Mellon University
# Copyright (C) 2013 Google Inc
# Written by Haakan Younes.
#
# Permission is hereby granted to distribute this software for
# non-commercial research purposes, provided that this copyright
# notice is included with any such distribution.
#
# THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
# SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
# ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

AC_INIT(VHPOP, 2.2.1, hlsyounes@gmail.com)
AC_CONFIG_SRCDIR(vhpop.cc)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_YACC

# Checks for libraries.
AC_SEARCH_LIBS(gettext, intl)

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset select strcasecmp strerror strncasecmp])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
