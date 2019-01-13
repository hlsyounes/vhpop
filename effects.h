// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Effects.

#ifndef EFFECTS_H
#define EFFECTS_H

#include <vector>

#include "terms.h"

struct Formula;
struct Literal;
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
  void set_condition(const Formula& condition);

  /* Sets the link condition of this effect. */
  void set_link_condition(const Formula& link_condition) const;

  /* Returns the number of universally quantified variables of this effect. */
  size_t arity() const { return parameters_.size(); }

  /* Returns the ith universally quantified variable of this effect. */
  Variable parameter(size_t i) const { return parameters_[i]; }

  /* Returns the condition for this effect. */
  const Formula& condition() const { return *condition_; }

  /* Returns the condition that must hold for this effect to be
     considered for linking. */
  const Formula& link_condition() const { return *link_condition_; }

  /* Returns the literal added by this effect. */
  const Literal& literal() const { return *literal_; }

  /* Returns the temporal annotation for this effect. */
  EffectTime when() const { return when_; }

  /* Tests if this effect quantifies the given variable. */
  bool quantifies(Variable variable) const;

  /* Fills the provided list with instantiations of this effect. */
  void instantiations(EffectList& effects, size_t& useful,
                      const std::map<Variable, Term>& subst,
                      const Problem& problem) const;

  /* Prints this effect on the given stream. */
  void print(std::ostream& os) const;

private:
  /* List of universally quantified variables for this effect. */
  std::vector<Variable> parameters_;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Formula* condition_;
  /* Condition that must hold for this effect to be considered for linking. */
  mutable const Formula* link_condition_;
  /* Litteral added by this effect. */
  const Literal* literal_;
  /* Temporal annotation for this effect. */
  EffectTime when_;

  /* Returns an instantiation of this effect. */
  const Effect* instantiation(const std::map<Variable, Term>& args,
                              const Problem& problem,
                              const Formula& condition) const;
};


/* ====================================================================== */
/* EffectList */

/*
 * List of effects.
 */
struct EffectList : public std::vector<const Effect*> {
};


#endif /* EFFECTS_H */
