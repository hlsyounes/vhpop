/* -*-C++-*- */
/*
 * PDDL requirements.
 *
 * $Id: requirements.h,v 1.1 2001-07-29 18:10:36 lorens Exp $
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
  Requirements()
    : strips(true), typing(false), disjunctive_preconditions(false),
      equality(false), existential_preconditions(false),
      universal_preconditions(false), conditional_effects(false) {
  }

  /* Enables quantified preconditions. */
  void quantified_preconditions() {
    existential_preconditions = true;
    universal_preconditions = true;
  }

  /* Enables ADL style actions. */
  void adl() {
    strips = true;
    typing = true;
    disjunctive_preconditions = true;
    equality = true;
    quantified_preconditions();
    conditional_effects = true;
  }
};

#endif /* REQUIREMENTS_H */
