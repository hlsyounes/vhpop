/* -*-C++-*- */
/*
 * Plan flaws.
 *
 * Copyright (C) 2002-2004 Carnegie Mellon University
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
  virtual void print(std::ostream& os, const Bindings& bindings) const = 0;
};


/* ====================================================================== */
/* OpenCondition */

/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Formula& condition);

  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Literal& condition, FormulaTime when);

  /* Constructs an open condition. */
  OpenCondition(const OpenCondition& oc);

  /* Deletes this open condition. */
  virtual ~OpenCondition();

  /* Returns the step id. */
  size_t step_id() const { return step_id_; }

  /* Returns the open condition. */
  const Formula& condition() const { return *condition_; }

  /* Checks if this is a static open condition. */
  bool is_static() const;

  /* Returns a literal, or NULL if this is not a literal open
     condition. */
  const Literal* literal() const;

  /* Returns the time stamp associated with a literal open condition. */
  FormulaTime when() const { return when_; }

  /* Returns a inequality, or NULL if this is not an inequality open
     condition. */
  const Inequality* inequality() const;

  /* Returns a disjunction, or NULL if this is not a disjunctive open
     condition. */
  const Disjunction* disjunction() const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const Bindings& bindings) const;

private:
  /* Id of step to which this open condition belongs. */
  size_t step_id_;
  /* The open condition. */
  const Formula* condition_;
  /* Time stamp associated with a literal open condition. */
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
  Unsafe(const Link& link, size_t step_id, const Effect& effect)
    : link_(&link), step_id_(step_id), effect_(&effect) {}

  /* Returns the threatened link. */
  const Link& link() const { return *link_; }

  /* Returns the id of threatening step. */
  size_t step_id() const { return step_id_; }

  /* Returns the threatening effect. */
  const Effect& effect() const { return *effect_; }

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const Bindings& bindings) const;

private:
  /* Threatened link. */
  const Link* link_;
  /* Id of threatening step. */
  size_t step_id_;
  /* Threatening effect. */
  const Effect* effect_;
};

/* Equality operator for unsafe links. */
inline bool operator==(const Unsafe& u1, const Unsafe& u2) {
  return &u1 == &u2;
}


/* ====================================================================== */
/* MutexThreat */

/*
 * A mutex threat between effects of two separate steps.
 */
struct MutexThreat : public Flaw {
  /* Constructs a mutex threat place hoder. */
  MutexThreat() : step_id1_(0) {}

  /* Constructs a mutex threat. */
  MutexThreat(size_t step_id1, const Effect& effect1,
	      size_t step_id2, const Effect& effect2)
    : step_id1_(step_id1), effect1_(&effect1),
      step_id2_(step_id2), effect2_(&effect2) {}

  /* Returns the id for the first step. */
  size_t step_id1() const { return step_id1_; }

  /* Returns the threatening effect for the first step. */
  const Effect& effect1() const { return *effect1_; }

  /* Returns the id for the second step. */
  size_t step_id2() const { return step_id2_; }

  /* Returns the threatening effect for the second step. */
  const Effect& effect2() const { return *effect2_; }

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const Bindings& bindings) const;

private:
  /* The id for the first step. */
  size_t step_id1_;
  /* The threatening effect for the first step. */
  const Effect* effect1_;
  /* The id for the second step. */
  size_t step_id2_;
  /* The threatening effect for the second step. */
  const Effect* effect2_;
};

/* Equality operator for mutex threats. */
inline bool operator==(const MutexThreat& mt1, const MutexThreat& mt2) {
  return &mt1 == &mt2;
}


#endif /* FLAWS_H */
