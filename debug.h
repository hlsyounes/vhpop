/* -*-C++-*- */
/*
 * Debugging related stuff.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: debug.h,v 1.8 2003-03-01 18:52:55 lorens Exp $
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <config.h>
#include <cstdlib>


/* Verbosity level. */
extern int verbosity;

#ifdef DEBUG_MEMORY
extern size_t created_variables;
extern size_t deleted_variables;
extern size_t created_formulas;
extern size_t deleted_formulas;
extern size_t created_name_sets;
extern size_t deleted_name_sets;
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
extern size_t created_chains;
extern size_t deleted_chains;
extern size_t created_reasons;
extern size_t deleted_reasons;
#endif


#endif /* DEBUG_H */
