/* -*-C++-*- */
/*
 * Planning parameters.
 *
 * $Id: parameters.h,v 1.7 2002-01-06 21:46:03 lorens Exp $
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
