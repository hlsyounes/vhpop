/*
 * Copyright (C) 2002 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: flaws.cc,v 3.2 2002-03-18 12:07:56 lorens Exp $
 */
#include "flaws.h"
#include "plans.h"
#include "reasons.h"
#include "domains.h"
#include "formulas.h"


/* ====================================================================== */
/* OpenCondition */

/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Reason& reason)
  : step_id_(step_id) {
#ifdef TRANSFORMATIONAL
  reason_ = &reason;
#endif
}


/* Returns the reason. */
const Reason& OpenCondition::reason() const {
#ifdef TRANSFORMATIONAL
  return *reason_;
#else
  return Reason::DUMMY;
#endif
}


/* Prints this open condition on the given stream. */
void OpenCondition::print(ostream& os) const {
  os << "#<OPEN " << condition() << " step " << step_id() << ">";
}


/* ====================================================================== */
/* LiteralOpenCondition */

/* Constructs a literal open condition. */
LiteralOpenCondition::LiteralOpenCondition(const Literal& cond,
					   size_t step_id,
					   const Reason& reason)
  : OpenCondition(step_id, reason), literal_(&cond) {}


/* Returns the open condition. */
const Formula& LiteralOpenCondition::condition() const {
  return literal();
}


/* Checks if this is a static open condition. */
bool LiteralOpenCondition::is_static(const Domain& domain) const {
  return (step_id() != Plan::GOAL_ID
	  && domain.static_predicate(literal().predicate()));
}


/* ====================================================================== */
/* InequalityOpenCondition */

/* Constructs an inequality open condition. */
InequalityOpenCondition::InequalityOpenCondition(const Inequality& cond,
						 size_t step_id,
						 const Reason& reason)
  : OpenCondition(step_id, reason), neq_(&cond) {}


/* Returns the open condition. */
const Formula& InequalityOpenCondition::condition() const {
  return neq();
}


/* Checks if this is a static open condition. */
bool InequalityOpenCondition::is_static(const Domain& domain) const {
  return false;
}


/* ====================================================================== */
/* DisjunctiveOpenCondition */

/* Constructs a disjunctive open condition. */
DisjunctiveOpenCondition::DisjunctiveOpenCondition(const Disjunction& cond,
						   size_t step_id,
						   const Reason& reason)
  : OpenCondition(step_id, reason), disjunction_(&cond) {}


/* Returns the open condition. */
const Formula& DisjunctiveOpenCondition::condition() const {
  return disjunction();
}


/* Checks if this is a static open condition. */
bool DisjunctiveOpenCondition::is_static(const Domain& domain) const {
  return false;
}


/* ====================================================================== */
/* Unsafe */

/* Constructs a threatened causal link. */
Unsafe::Unsafe(const Link& link, size_t step_id, const Effect& effect,
	       const Literal& effect_add)
  : link_(&link), step_id_(step_id),
    effect_(&effect), effect_add_(&effect_add) {}


/* Prints this threatened causal link on the given stream. */
void Unsafe::print(ostream& os) const {
  os << "#<UNSAFE " << link().from_id() << ' ' << link().condition()
     << ' ' << link().to_id() << " step " << step_id() << ">";
}
