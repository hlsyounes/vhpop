/*
 * $Id: flaws.cc,v 1.4 2001-12-29 16:39:23 lorens Exp $
 */
#include "flaws.h"
#include "plans.h"
#include "domains.h"
#include "formulas.h"


/* ====================================================================== */
/* OpenCondition */

/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Reason& reason)
  : step_id(step_id), reason(reason) {}


/* Prints this open condition on the given stream. */
void OpenCondition::print(ostream& os) const {
  os << "#<OPEN " << condition() << " step " << step_id << ">";
}


/* ====================================================================== */
/* LiteralOpenCondition */

/* Constructs a literal open condition. */
LiteralOpenCondition::LiteralOpenCondition(const Literal& cond,
					   size_t step_id,
					   const Reason& reason)
  : OpenCondition(step_id, reason), literal(cond) {}


/* Returns the open condition. */
const Formula& LiteralOpenCondition::condition() const {
  return literal;
}


/* Checks if this is a static open condition. */
bool LiteralOpenCondition::is_static(const Domain& domain) const {
  return domain.static_predicate(literal.predicate());
}


/* ====================================================================== */
/* InequalityOpenCondition */

/* Constructs an inequality open condition. */
InequalityOpenCondition::InequalityOpenCondition(const Inequality& cond,
						 size_t step_id,
						 const Reason& reason)
  : OpenCondition(step_id, reason), neq(cond) {}


/* Returns the open condition. */
const Formula& InequalityOpenCondition::condition() const {
  return neq;
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
  : OpenCondition(step_id, reason), disjunction(cond) {}


/* Returns the open condition. */
const Formula& DisjunctiveOpenCondition::condition() const {
  return disjunction;
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
  : link(link), step_id(step_id), effect(effect), effect_add(effect_add) {}


/* Prints this threatened causal link on the given stream. */
void Unsafe::print(ostream& os) const {
  os << "#<UNSAFE " << link << " step " << step_id << ">";
}
