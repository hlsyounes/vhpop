/* -*-C++-*- */
/*
 * PDDL requirements.
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
 * $Id: requirements.h,v 1.3 2002-01-25 18:23:55 lorens Exp $
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
