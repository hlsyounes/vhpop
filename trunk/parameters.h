/* -*-C++-*- */
/*
 * Planning parameters.
 *
 * $Id: parameters.h,v 1.4 2001-12-30 16:16:12 lorens Exp $
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "support.h"
#include "heuristics.h"


/*
 * An invalid search algorithm exception.
 */
struct InvalidSearchAlgorithm : public Exception {
  /* Constructs an invalid search algorithm exception. */
  InvalidSearchAlgorithm(const string& name);
};


/*
 * Planning parameters.
 */
struct Parameters : public gc {
  /* Valid search algorithms. */
  typedef enum { A_STAR, IDA_STAR, HILL_CLIMBING } SearchAlgorithm;

  /* Search limit. */
  size_t search_limit;
  /* Time limit, in minutes. */
  size_t time_limit;
  /* Search algorithm to use. */
  SearchAlgorithm search_algorithm;
  /* Whether to use ground actions. */
  bool ground_actions;
  /* Whether to use transformational planner. */
  bool transformational;
  /* Plan selection heuristic. */
  Heuristic heuristic;
  /* Weight to use with heuristic. */
  double weight;
  /* Flaw selecion order. */
  FlawSelectionOrder flaw_order;
  /* Whether to use parameter domain constraints. */
  bool domain_constraints;

  /* Constructs default planning parameters. */
  Parameters();

  /* Selects a search algorithm from a name. */
  void set_search_algorithm(const string& name);
};

#endif /* PARAMETERS_H */
