/* -*-C++-*- */
/*
 * PDDL requirements.
 *
 * $Id: requirements.h,v 1.2 2001-12-27 20:08:59 lorens Exp $
 */
#ifndef REQUIREMENTS_H
#define REQUIREMENTS_H

#include "support.h"


/*
 * PDDL requirements.
 */
struct Requirements : public gc {
  /* Required action style. */
  bool strips;
  /* Whether support for types is required. */
  bool typing;
  /* Whether support for disjunctive preconditions is required. */
  bool disjunctive_preconditions;
  /* Whether support for equality predicate is required. */
  bool equality;
  /* Whether support for existentially quantified preconditions is
     required. */
  bool existential_preconditions;
  /* Whether support for universally quantified preconditions is
     required. */
  bool universal_preconditions;
  /* Whether support for conditional effects is required. */
  bool conditional_effects;

  /* Constructs a default requirements object. */
  Requirements();

  /* Enables quantified preconditions. */
  void quantified_preconditions();

  /* Enables ADL style actions. */
  void adl();
};


#endif /* REQUIREMENTS_H */
