/* -*-C++-*- */
/*
 * Planning parameters.
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
 * $Id: parameters.h,v 3.2 2002-03-18 13:56:54 lorens Exp $
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "support.h"

struct Heuristic;
struct FlawSelectionOrder;


/* ====================================================================== */
/* InvalidSearchAlgorithm */

/*
 * An invalid search algorithm exception.
 */
struct InvalidSearchAlgorithm : public Exception {
  /* Constructs an invalid search algorithm exception. */
  InvalidSearchAlgorithm(const string& name);
};


/* ====================================================================== */
/* Parameters */

/*
 * Planning parameters.
 */
struct Parameters {
  /* Valid search algorithms. */
  typedef enum { A_STAR, IDA_STAR, HILL_CLIMBING } SearchAlgorithm;

  /* Search limit. */
  size_t search_limit;
  /* Time limit, in minutes. */
  size_t time_limit;
  /* Search algorithm to use. */
  SearchAlgorithm search_algorithm;
  /* Plan selection heuristic. */
  Heuristic& heuristic;
  /* Weight to use with heuristic. */
  double weight;
  /* Flaw selecion order. */
  FlawSelectionOrder& flaw_order;
  /* Whether to reverse the order that open conditions are added. */
  bool reverse_open_conditions;
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

  /* Selects a search algorithm from a name. */
  void set_search_algorithm(const string& name);
};


#endif /* PARAMETERS_H */
