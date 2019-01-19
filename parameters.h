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
// Planning parameters.

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

#include "heuristics.h"

/* ====================================================================== */
/* InvalidSearchAlgorithm */

/*
 * An invalid search algorithm exception.
 */
struct InvalidSearchAlgorithm : public std::runtime_error {
  /* Constructs an invalid search algorithm exception. */
  InvalidSearchAlgorithm(const std::string& name);
};


/* ====================================================================== */
/* InvalidActionCost */

/*
 * An invalid action cost exception.
 */
struct InvalidActionCost : public std::runtime_error {
  /* Constructs an invalid action cost exception. */
  InvalidActionCost(const std::string& name);
};


/* ====================================================================== */
/* Parameters */

/*
 * Planning parameters.
 */
struct Parameters {
  /* Valid search algorithms. */
  typedef enum { A_STAR, IDA_STAR, HILL_CLIMBING } SearchAlgorithm;
  /* Valid action costs. */
  typedef enum { UNIT_COST, DURATION, RELATIVE } ActionCost;

  /* Time limit. */
  std::chrono::nanoseconds time_limit;
  /* Search algorithm to use. */
  SearchAlgorithm search_algorithm;
  /* Plan selection heuristic. */
  Heuristic heuristic;
  /* Action cost. */
  ActionCost action_cost;
  /* Weight to use with heuristic. */
  float weight;
  /* Flaw selecion orders. */
  std::vector<FlawSelectionOrder> flaw_orders;
  /* Search limits. */
  std::vector<size_t> search_limits;
  /* Whether to add open conditions in random order. */
  bool random_open_conditions;
  /* Whether to use ground actions. */
  bool ground_actions;
  /* Whether to use parameter domain constraints. */
  bool domain_constraints;
  /* Whether to keep static preconditions when using domain constraints. */
  bool keep_static_preconditions;

  /* Constructs default planning parameters. */
  Parameters();

  /* Whether to strip static preconditions. */
  bool strip_static_preconditions() const;

  /* Selects a search algorithm from a name. */
  void set_search_algorithm(const std::string& name);

  /* Selects an action cost from a name. */
  void set_action_cost(const std::string& name);
};


#endif /* PARAMETERS_H */
