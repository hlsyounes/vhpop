/* -*-C++-*- */
/*
 * Effects.
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
 * $Id: effects.h,v 6.1 2003-07-28 21:39:43 lorens Exp $
 */
#ifndef EFFECTS_H
#define EFFECTS_H

#include <config.h>
#include "terms.h"
#include <vector>

struct PredicateTable;
struct Literal;
struct Condition;
struct Problem;


/* ====================================================================== */
/* Effect */

struct EffectList;

/*
 * A single effect.
 */
struct Effect {
  /* Possible temporal annotations for effects. */
  typedef enum { AT_START, AT_END } EffectTime;

  /* Constructs an effect. */
  Effect(const Literal& literal, EffectTime when);

  /* Deletes this effect. */
  ~Effect();

  /* Adds a universally quantified variable to this effect. */
  void add_parameter(Variable parameter);

  /* Sets the condition of this effect. */
  void set_condition(const Condition& condition);

  /* Sets the link condition of this effect. */
  void set_link_condition(const Condition& link_condition) const;

  /* Returns the number of universally quantified variables of this effect. */
  size_t arity() const { return parameters_.size(); }

  /* Returns the ith universally quantified variable of this effect. */
  Variable parameter(size_t i) const { return parameters_[i]; }

  /* Returns the condition for this effect. */
  const Condition& condition() const { return *condition_; }

  /* Returns the condition that must hold for this effect to be
     considered for linking. */
  const Condition& link_condition() const { return *link_condition_; }

  /* Returns the literal added by this effect. */
  const Literal& literal() const { return *literal_; }

  /* Returns the temporal annotation for this effect. */
  EffectTime when() const { return when_; }

  /* Tests if this effect quantifies the given variable. */
  bool quantifies(Variable variable) const;

  /* Fills the provided list with instantiations of this effect. */
  void instantiations(EffectList& effects, size_t& useful,
		      const SubstitutionMap& subst,
		      const Problem& problem) const;

  /* Prints this effect on the given stream. */
  void print(std::ostream& os, const PredicateTable& predicates,
	     const TermTable& terms) const;

private:
  /* List of universally quantified variables for this effect. */
  VariableList parameters_;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Condition* condition_;
  /* Condition that must hold for this effect to be considered for linking. */
  mutable const Condition* link_condition_;
  /* Litteral added by this effect. */
  const Literal* literal_;
  /* Temporal annotation for this effect. */
  EffectTime when_;

  /* Returns an instantiation of this effect. */
  const Effect* instantiation(const SubstitutionMap& args,
			      const Problem& problem,
			      const Condition& condition) const;
};


/* ====================================================================== */
/* EffectList */

/*
 * List of effects.
 */
struct EffectList : public std::vector<const Effect*> {
};


#endif /* EFFECTS_H */
