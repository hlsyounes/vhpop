/* -*-C++-*- */
/*
 * Planning parameters.
 *
 * $Id: parameters.h,v 1.3 2001-12-23 22:08:40 lorens Exp $
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "support.h"
#include "heuristics.h"


/*
 * Planning parameters.
 */
struct Parameters : public gc {
  /* Search limit. */
  size_t search_limit;
  /* Time limit, in minutes. */
  size_t time_limit;
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

  Parameters()
    : search_limit(2000), time_limit(1440), ground_actions(false),
      transformational(false), heuristic("SUM"), weight(1.0),
      flaw_order("LIFO"), domain_constraints(false) {
  }
};

#endif /* PARAMETERS_H */