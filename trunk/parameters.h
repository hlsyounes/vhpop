/* -*-C++-*- */
/*
 * Planning parameters.
 *
 * $Id: parameters.h,v 1.5 2002-01-03 12:40:15 lorens Exp $
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "support.h"

struct Heuristic;
struct FlawSelectionOrder;


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
  /* Plan selection heuristic. */
  Heuristic& heuristic;
  /* Weight to use with heuristic. */
  double weight;
  /* Flaw selecion order. */
  FlawSelectionOrder& flaw_order;
  /* Whether to use ground actions. */
  bool ground_actions;
  /* Whether to use parameter domain constraints. */
  bool domain_constraints;
  /* Whether to use transformational planner. */
  bool transformational;

  /* Constructs default planning parameters. */
  Parameters();

  /* Selects a search algorithm from a name. */
  void set_search_algorithm(const string& name);
};

#endif /* PARAMETERS_H */
