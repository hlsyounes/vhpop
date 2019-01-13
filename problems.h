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
// Problem descriptions.

#ifndef PROBLEMS_H
#define PROBLEMS_H

#include "domains.h"
#include "formulas.h"
#include "expressions.h"
#include "terms.h"
#include "types.h"


/* ====================================================================== */
/* TimedActionTable */

/*
 * A table of timed actions.
 */
struct TimedActionTable : public std::map<float, GroundAction*> {
};


/* ====================================================================== */
/* Problem */

/*
 * Problem definition.
 */
struct Problem {
  /* Table of problem definitions. */
  struct ProblemMap : public std::map<std::string, const Problem*> {
  };

  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMap::const_iterator begin();

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMap::const_iterator end();

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const std::string& name);

  /* Removes all defined problems. */
  static void clear();

  /* Constructs a problem. */
  Problem(const std::string& name, const Domain& domain);

  /* Deletes a problem. */
  ~Problem();

  /* Returns the name of this problem. */
  const std::string& name() const { return name_; }

  /* Returns the domain of this problem. */
  const Domain& domain() const { return *domain_; }

  /* Returns the term table of this problem. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this problem. */
  const TermTable& terms() const { return terms_; }

  /* Adds an atomic formula to the initial conditions of this problem. */
  void add_init_atom(const Atom& atom);

  /* Adds a timed initial literal to this problem. */
  void add_init_literal(float time, const Literal& literal);

  /* Adds a fluent value to the initial conditions of this problem. */
  void add_init_value(const Fluent& fluent, float value);

  /* Sets the goal of this problem. */
  void set_goal(const Formula& goal);

  /* Sets the metric to minimize for this problem. */
  void set_metric(const Expression& metric, bool negate = false);

  /* Returns the initial atoms of this problem. */
  const AtomSet& init_atoms() const { return init_atoms_; }

  /* Returns the initial values of this problem. */
  const ValueMap& init_values() const { return init_values_; }

  /* Returns the action representing the initial conditions of this problem. */
  const GroundAction& init_action() const { return init_action_; }

  /* Returns the actions representing the timed initial literals. */
  const TimedActionTable& timed_actions() const { return timed_actions_; }

  /* Returns the goal of this problem. */
  const Formula& goal() const { return *goal_; }

  /* Returns the metric to minimize for this problem. */
  const Expression& metric() const { return *metric_; }

  /* Tests if the metric is constant. */
  bool constant_metric() const;

  /* Fills the provided action list with ground actions instantiated
     from the action schemas of the domain. */
  void instantiated_actions(std::vector<const GroundAction*>& actions) const;

 private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Name of problem. */
  std::string name_;
  /* Problem domain. */
  const Domain* domain_;
  /* Problem terms. */
  mutable TermTable terms_;
  /* Initial atoms. */
  AtomSet init_atoms_;
  /* Initial fluent values. */
  ValueMap init_values_;
  /* Aciton representing initial conditions of problem. */
  GroundAction init_action_;
  /* Actions representing timed initial conditions of problem. */
  TimedActionTable timed_actions_;
  /* Goal of problem. */
  const Formula* goal_;
  /* Metric to minimize. */
  const Expression* metric_;

  friend std::ostream& operator<<(std::ostream& os, const Problem& p);
};

/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p);


#endif /* PROBLEMS_H */
