/*
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
 * $Id: parameters.cc,v 6.3 2003-12-05 23:16:43 lorens Exp $
 */
#include "parameters.h"


/* ====================================================================== */
/* InvalidSearchAlgorithm */

/* Constructs an invalid search algorithm exception. */
InvalidSearchAlgorithm::InvalidSearchAlgorithm(const std::string& name)
  : std::runtime_error("invalid search algorithm `" + name + "'") {}


/* ====================================================================== */
/* InvalidActionCost */

/* Constructs an invalid action cost exception. */
InvalidActionCost::InvalidActionCost(const std::string& name)
  : std::runtime_error("invalid action cost `" + name + "'") {}


/* ====================================================================== */
/* Parameters */

/* Constructs default planning parameters. */
Parameters::Parameters()
  : time_limit(UINT_MAX), search_algorithm(A_STAR),
    heuristic("UCPOP"), action_cost(UNIT_COST), weight(1.0),
    random_open_conditions(false), ground_actions(false),
    domain_constraints(false), keep_static_preconditions(true) {
  flaw_orders.push_back(FlawSelectionOrder("UCPOP")),
  search_limits.push_back(UINT_MAX);
}


/* Whether to strip static preconditions. */
bool Parameters::strip_static_preconditions() const {
  return !ground_actions && domain_constraints && !keep_static_preconditions;
}


/* Selects a search algorithm from a name. */
void Parameters::set_search_algorithm(const std::string& name) {
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


/* Selects an action cost from a name. */
void Parameters::set_action_cost(const std::string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "UNIT") == 0) {
    action_cost = UNIT_COST;
  } else if (strcasecmp(n, "DURATION") == 0) {
    action_cost = DURATION;
  } else if (strcasecmp(n, "RELATIVE") == 0) {
    action_cost = RELATIVE;
  } else {
    throw InvalidActionCost(name);
  }
}
