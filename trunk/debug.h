/* -*-C++-*- */
/*
 * Debugging related stuff.
 *
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: debug.h,v 1.4 2002-03-29 10:04:11 lorens Exp $
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <cstdlib>


/* Verbosity level. */
extern int verbosity;

#ifdef DEBUG_MEMORY
extern size_t created_plans;
extern size_t deleted_plans;
extern size_t created_chains;
extern size_t deleted_chains;
extern size_t created_collectibles;
extern size_t deleted_collectibles;
#endif


#endif /* DEBUG_H */
