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

#include "parameters.h"

#include <limits>
#include <string.h>

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
    : time_limit(std::chrono::nanoseconds::max()),
      search_algorithm(A_STAR),
      heuristic("UCPOP"),
      action_cost(UNIT_COST),
      weight(1.0),
      random_open_conditions(false),
      ground_actions(false),
      domain_constraints(false),
      keep_static_preconditions(true) {
  flaw_orders.push_back(FlawSelectionOrder("UCPOP")),
  search_limits.push_back(std::numeric_limits<unsigned int>::max());
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
