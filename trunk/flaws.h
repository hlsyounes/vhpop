/* -*-C++-*- */
/*
 * Plan flaws.
 *
 * $Id: flaws.h,v 1.1 2001-12-29 16:39:20 lorens Exp $
 */
#ifndef FLAWS_H
#define FLAWS_H

#include "support.h"

struct Formula;
struct Literal;
struct Inequality;
struct Disjunction;
struct Domain;
struct Reason;
struct Effect;
struct Link;


/*
 * Abstract flaw.
 */
struct Flaw : public Printable, public gc {
};


/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Id of step to which this open condition belongs. */
  const size_t step_id;
  /* Reason for open condition. */
  const Reason& reason;

  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const = 0;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const = 0;

protected:
  /* Prints this open condition on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * A literal open condition.
 */
struct LiteralOpenCondition : public OpenCondition {
  /* Literal. */
  const Literal& literal;

  /* Constructs a literal open condition. */
  LiteralOpenCondition(const Literal& cond, size_t step_id,
		       const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;
};


/*
 * An inequality open condition.
 */
struct InequalityOpenCondition : public OpenCondition {
  /* Inequality. */
  const Inequality& neq;

  /* Constructs an inequality open condition. */
  InequalityOpenCondition(const Inequality& cond, size_t step_id,
			  const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;
};


/*
 * A disjunctive open condition.
 */
struct DisjunctiveOpenCondition : public OpenCondition {
  /* Disjunction. */
  const Disjunction& disjunction;

  /* Constructs a disjunctive open condition. */
  DisjunctiveOpenCondition(const Disjunction& cond, size_t step_id,
			   const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;
};


/*
 * Threatened causal link.
 */
struct Unsafe : public Flaw {
  /* Threatened link. */
  const Link& link;
  /* Id of threatening step. */
  const size_t step_id;
  /* Threatening effect. */
  const Effect& effect;
  /* Specific part of effect that threatens link. */
  const Literal& effect_add;

  /* Constructs a threatened causal link. */
  Unsafe(const Link& link, size_t step_id, const Effect& effect,
	 const Literal& effect_add);

protected:
  /* Prints this threatened causal link on the given stream. */
  virtual void print(ostream& os) const;
};


#endif /* FLAWS_H */
