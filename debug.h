// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Debugging related stuff.

#ifndef DEBUG_H
#define DEBUG_H

#include <config.h>
#include <cstdlib>


/* Verbosity level. */
extern int verbosity;

#ifdef DEBUG_MEMORY
extern size_t created_formulas;
extern size_t deleted_formulas;
extern size_t created_conditions;
extern size_t deleted_conditions;
extern size_t created_action_domains;
extern size_t deleted_action_domains;
extern size_t created_bindings;
extern size_t deleted_bindings;
extern size_t created_bool_vectors;
extern size_t deleted_bool_vectors;
extern size_t created_float_vectors;
extern size_t deleted_float_vectors;
extern size_t created_orderings;
extern size_t deleted_orderings;
extern size_t created_plans;
extern size_t deleted_plans;
#endif


#endif /* DEBUG_H */
