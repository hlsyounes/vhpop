/*
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
 * $Id: parameters.cc,v 3.5 2002-09-24 17:37:27 lorens Exp $
 */
#include "parameters.h"


/* ====================================================================== */
/* InvalidSearchAlgorithm */

/* Constructs an invalid search algorithm exception. */
InvalidSearchAlgorithm::InvalidSearchAlgorithm(const string& name)
  : Exception("invalid search algorithm `" + name + "'") {}


/* ====================================================================== */
/* Parameters */

/* Constructs default planning parameters. */
Parameters::Parameters()
  : time_limit(UINT_MAX), search_algorithm(A_STAR),
    heuristic("UCPOP"), weight(1.0),
    reverse_open_conditions(false), ground_actions(false),
    domain_constraints(false), keep_static_preconditions(true),
    transformational(false) {
  flaw_orders.push_back(FlawSelectionOrder("UCPOP")),
  search_limits.push_back(UINT_MAX);
}


/* Whether to strip static preconditions. */
bool Parameters::strip_static_preconditions() const {
  return !ground_actions && domain_constraints && !keep_static_preconditions;
}


/* Selects a search algorithm from a name. */
void Parameters::set_search_algorithm(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "A") == 0) {
    search_algorithm = A_STAR;
  } else if (strcasecmp(n, "IDA") == 0) {
    search_algorithm = IDA_STAR;
  } else if (strcasecmp(n, "HC") == 0) {
    search_algorithm = HILL_CLIMBING;
  } else {
    throw InvalidSearchAlgorithm(name);
  }
}
