/*
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
 * $Id: flaws.cc,v 3.3 2002-03-23 15:18:20 lorens Exp $
 */
#include "flaws.h"
#include "plans.h"
#include "reasons.h"
#include "domains.h"
#include "formulas.h"


/* ====================================================================== */
/* OpenCondition */

/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Formula& condition,
			     const Reason& reason)
  : step_id_(step_id), condition_(&condition) {
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


/* Checks if this is a static open condition. */
bool OpenCondition::is_static(const Domain& domain) const {
  const Literal* lit = literal();
  return (lit != NULL && step_id() != Plan::GOAL_ID
	  && domain.static_predicate(lit->predicate()));
}


/* Returns a literal, or NULL if this is not a literal open
   condition. */
const Literal* OpenCondition::literal() const {
  return dynamic_cast<const Literal*>(condition_);
}


/* Returns a inequality, or NULL if this is not an inequality open
   condition. */
const Inequality* OpenCondition::inequality() const {
  return dynamic_cast<const Inequality*>(condition_);
}


/* Returns a disjunction, or NULL if this is not a disjunctive open
   condition. */
const Disjunction* OpenCondition::disjunction() const {
  return dynamic_cast<const Disjunction*>(condition_);
}


/* Prints this open condition on the given stream. */
void OpenCondition::print(ostream& os) const {
  os << "#<OPEN " << condition() << " step " << step_id() << ">";
}


/* Equality operator for open conditions. */
bool operator==(const OpenCondition& oc1, const OpenCondition& oc2) {
  return &oc1 == &oc2;
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


/* Equality operator for unsafe links. */
bool operator==(const Unsafe& u1, const Unsafe& u2) {
  return &u1 == &u2;
}
