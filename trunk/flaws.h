/* -*-C++-*- */
/*
 * Plan flaws.
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
 * $Id: flaws.h,v 6.3 2003-07-21 02:18:14 lorens Exp $
 */
#ifndef FLAWS_H
#define FLAWS_H

#include <config.h>
#include "formulas.h"
#include "chain.h"
#include <iostream>

struct Domain;
struct Effect;
struct Link;


/* ====================================================================== */
/* Flaw */

/*
 * Abstract flaw.
 */
struct Flaw {
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms,
		     const Bindings& bindings) const = 0;
};


/* ====================================================================== */
/* OpenCondition */

/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Formula& condition, FormulaTime when);

  /* Constructs an open condition. */
  OpenCondition(const OpenCondition& oc);

  /* Deletes this open condition. */
  virtual ~OpenCondition();

  /* Returns the step id. */
  size_t step_id() const { return step_id_; }

  /* Returns the open condition. */
  const Formula& condition() const { return *condition_; }

  /* Returns the time of this condition. */
  FormulaTime when() const { return when_; }

  /* Checks if this is a static open condition. */
  bool is_static(const Domain& domain) const;

  /* Returns a literal, or NULL if this is not a literal open
     condition. */
  const Literal* literal() const;

  /* Returns a inequality, or NULL if this is not an inequality open
     condition. */
  const Inequality* inequality() const;

  /* Returns a disjunction, or NULL if this is not a disjunctive open
     condition. */
  const Disjunction* disjunction() const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicate,
		     const TermTable& terms,
		     const Bindings& bindings) const;

private:
  /* Id of step to which this open condition belongs. */
  size_t step_id_;
  /* The open condition. */
  const Formula* condition_;
  /* The time of the condition. */
  FormulaTime when_;
};

/* Equality operator for open conditions. */
inline bool operator==(const OpenCondition& oc1, const OpenCondition& oc2) {
  return &oc1 == &oc2;
}


/* ====================================================================== */
/* Unsafe */

/*
 * Threatened causal link.
 */
struct Unsafe : public Flaw {
  /* Constructs a threatened causal link. */
  Unsafe(const Link& link, size_t step_id, const Effect& effect,
	 const Literal& effect_add);

  /* Returns the threatened link. */
  const Link& link() const { return *link_; }

  /* Returns the id of threatening step. */
  size_t step_id() const { return step_id_; }

  /* Returns the threatening effect. */
  const Effect& effect() const { return *effect_; }

  /* Returns the specific part of effect that threatens link. */
  const Literal& effect_add() const { return *effect_add_; }

  /* Prints this open condition on the given stream. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms,
		     const Bindings& bindings) const;

private:
  /* Threatened link. */
  const Link* link_;
  /* Id of threatening step. */
  size_t step_id_;
  /* Threatening effect. */
  const Effect* effect_;
  /* Specific part of effect that threatens link. */
  const Literal* effect_add_;
};

/* Equality operator for unsafe links. */
inline bool operator==(const Unsafe& u1, const Unsafe& u2) {
  return &u1 == &u2;
}


#endif /* FLAWS_H */
