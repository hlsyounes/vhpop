/* -*-C++-*- */
/*
 * Planning parameters.
 *
 * $Id: parameters.h,v 1.1 2001-09-29 18:55:37 lorens Exp $
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
  /* Whether to use ground actions. */
  bool ground_actions;
  /* Whether to use transformational planner. */
  bool transformational;
  /* Plan selection heuristic. */
  Heuristic heuristic;
  /* Weight to use with heuristic. */
  int weight;
  /* Flaw selecion order. */
  FlawSelectionOrder flaw_order;

  Parameters()
    : search_limit(2000), ground_actions(false), transformational(false),
      heuristic("SUMR"), weight(1), flaw_order("LIFO") {
  }
};

#endif /* PARAMETERS_H */
