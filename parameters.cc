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
 * $Id: parameters.cc,v 1.5 2002-01-25 18:23:00 lorens Exp $
 */
#include "parameters.h"
#include "heuristics.h"


/* ====================================================================== */
/* InvalidSearchAlgorithm */

/* Constructs an invalid search algorithm exception. */
InvalidSearchAlgorithm::InvalidSearchAlgorithm(const string& name)
  : Exception("invalid search algorithm `" + name + "'") {}


/* ====================================================================== */
/* Parameters */

/* Constructs default planning parameters. */
Parameters::Parameters()
  : search_limit(2000), time_limit(1440), search_algorithm(A_STAR),
    heuristic(*(new Heuristic("SUM"))), weight(1.0),
    flaw_order(*(new FlawSelectionOrder("UCPOP"))),
    reverse_open_conditions(false), ground_actions(false),
    domain_constraints(false), keep_static_preconditions(true),
    transformational(false) {}


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
