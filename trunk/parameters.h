/* -*-C++-*- */
/*
 * Planning parameters.
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
 * $Id: parameters.h,v 6.1 2003-03-23 10:20:07 lorens Exp $
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <config.h>
#include "heuristics.h"
#include "exceptions.h"
#include <string>
#include <vector>


/* ====================================================================== */
/* InvalidSearchAlgorithm */

/*
 * An invalid search algorithm exception.
 */
struct InvalidSearchAlgorithm : public Exception {
  /* Constructs an invalid search algorithm exception. */
  InvalidSearchAlgorithm(const std::string& name);
};


/* ====================================================================== */
/* Parameters */

/*
 * Planning parameters.
 */
struct Parameters {
  /* Valid search algorithms. */
  typedef enum { A_STAR, IDA_STAR, HILL_CLIMBING } SearchAlgorithm;

  /* Time limit, in minutes. */
  size_t time_limit;
  /* Search algorithm to use. */
  SearchAlgorithm search_algorithm;
  /* Plan selection heuristic. */
  Heuristic heuristic;
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
  /* Whether to use transformational planner. */
  bool transformational;

  /* Constructs default planning parameters. */
  Parameters();

  /* Whether to strip static preconditions. */
  bool strip_static_preconditions() const;

  /* Selects a search algorithm from a name. */
  void set_search_algorithm(const std::string& name);
};


#endif /* PARAMETERS_H */
