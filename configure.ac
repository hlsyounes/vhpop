## Process this file with autoconf to produce a configure script.
##
## Copyright (C) 2002--2005 Carnegie Mellon University
## Copyright (C) 2019 Google Inc
##
## This file is part of VHPOP.
##
## VHPOP is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## VHPOP is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with VHPOP; if not, write to the Free Software Foundation,
## Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

AC_PREREQ(2.57)
AC_INIT(VHPOP, 3.0, hlsyounes@gmail.com)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(vhpop.cc)
AM_INIT_AUTOMAKE([subdir-objects])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AM_PROG_LEX
AC_PROG_LIBTOOL
AC_PROG_YACC

AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])

# Whether to use tcmalloc.
AC_ARG_WITH([tcmalloc],
            [AS_HELP_STRING([--with-tcmalloc],
                            [a malloc replacement @<:@default=no@:>@])],
            [],
            [with_tcmalloc=no])
AS_IF([test "x$with_tcmalloc" != xno],
      [AC_CHECK_LIB(
          [tcmalloc], [tc_malloc], ,
          [AS_IF([test "x$with_tcmalloc" != xcheck],
                 [AC_MSG_FAILURE(
                     [--with-tcmalloc was given, but test for tcmalloc failed])
                 ])])])

# Checks for libraries.
AC_SEARCH_LIBS(gettext, intl)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset strcasecmp strerror strncasecmp getopt_long])

AC_CONFIG_FILES(Makefile)
AC_CONFIG_SUBDIRS([gtest])
AC_OUTPUT
