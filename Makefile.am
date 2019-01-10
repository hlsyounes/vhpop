## Process this file with automake to produce Makefile.in
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

bin_PROGRAMS = vhpop
vhpop_SOURCES = vhpop.cc refcount.cc refcount.h chain.h types.cc types.h terms.cc terms.h predicates.cc predicates.h functions.cc functions.h expressions.cc expressions.h formulas.cc formulas.h effects.cc effects.h actions.cc actions.h domains.cc domains.h problems.cc problems.h bindings.cc bindings.h orderings.cc orderings.h flaws.cc flaws.h heuristics.cc heuristics.h plans.cc plans.h parameters.cc parameters.h requirements.cc requirements.h pddl.yy tokens.ll debug.h
vhpop_LDADD = @LIBOBJS@
MAINTAINERCLEANFILES = pddl.cc tokens.cc
EXTRA_DIST = getopt.c getopt1.c getopt.h ipc3-vhpop examples scripts

YFLAGS = -d
