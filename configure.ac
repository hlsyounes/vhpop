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

AC_INIT(VHPOP, 3.0, lorens@cs.cmu.edu)
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

AC_LANG(C++)

# Checks for libraries.
AC_SEARCH_LIBS(gettext, intl)

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h sys/time.h unistd.h sstream])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset select strcasecmp strerror strncasecmp])
AC_CHECK_FUNCS(getopt_long, , [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])
AC_CHECK_FUNCS(isinf, , [AC_EGREP_HEADER(finite, math.h, ,
                                         [AC_CHECK_HEADERS(ieeefp.h)])])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
