/* -*-C++-*- */
/*
 * Plan flaws.
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
 * $Id: flaws.h,v 3.1 2002-03-18 10:11:19 lorens Exp $
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


/* ====================================================================== */
/* Flaw */

/*
 * Abstract flaw.
 */
struct Flaw : public Printable, public gc {
};


/* ====================================================================== */
/* OpenCondition */

/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Reason& reason);

  /* Returns the step id. */
  size_t step_id() const { return step_id_; }

  /* Returns the reason. */
  const Reason& reason() const;

  /* Returns the open condition. */
  virtual const Formula& condition() const = 0;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const = 0;

protected:
  /* Prints this open condition on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Id of step to which this open condition belongs. */
  const size_t step_id_;
#ifdef TRANSFORMATIONAL
  /* Reason for open condition. */
  const Reason* reason_;
#endif
};


/* ====================================================================== */
/* LiteralOpenCondition */

/*
 * A literal open condition.
 */
struct LiteralOpenCondition : public OpenCondition {
  /* Constructs a literal open condition. */
  LiteralOpenCondition(const Literal& cond, size_t step_id,
		       const Reason& reason);

  /* Returns the literal. */
  const Literal& literal() const { return *literal_; }

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;

private:
  /* Literal. */
  const Literal* literal_;
};


/* ====================================================================== */
/* InequalityOpenCondition */

/*
 * An inequality open condition.
 */
struct InequalityOpenCondition : public OpenCondition {
  /* Constructs an inequality open condition. */
  InequalityOpenCondition(const Inequality& cond, size_t step_id,
			  const Reason& reason);

  /* Returns the inequality. */
  const Inequality& neq() const { return *neq_; }

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;

private:
  /* Inequality. */
  const Inequality* neq_;
};


/* ====================================================================== */
/* DisjunctiveOpenCondition */

/*
 * A disjunctive open condition.
 */
struct DisjunctiveOpenCondition : public OpenCondition {
  /* Constructs a disjunctive open condition. */
  DisjunctiveOpenCondition(const Disjunction& cond, size_t step_id,
			   const Reason& reason);

  /* Returns the disjunction. */
  const Disjunction& disjunction() const { return *disjunction_; }

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;

private:
  /* Disjunction. */
  const Disjunction* disjunction_;
};


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

protected:
  /* Prints this threatened causal link on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Threatened link. */
  const Link* link_;
  /* Id of threatening step. */
  const size_t step_id_;
  /* Threatening effect. */
  const Effect* effect_;
  /* Specific part of effect that threatens link. */
  const Literal* effect_add_;
};


#endif /* FLAWS_H */
