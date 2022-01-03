// Copyright (C) 2021 Google Inc
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
// PDDL requirements.

#ifndef PDDL_REQUIREMENTS_H_
#define PDDL_REQUIREMENTS_H_

class PddlRequirements {
 public:
  // Allow type names in declarations of variables.
  void EnableTyping();
  // Allow "not" in goal descriptions.
  void EnableNegativePreconditions();
  // Allow "or" in goal descriptions.
  void EnableDisjunctivePreconditions();
  // Support "=" as built-in predicate.
  void EnableEquality();
  // Allow "exists" in goal descriptions.
  void EnableExistentialPreconditions();
  // Allow "forall" in goal descriptions.
  void EnableUniversalPreconditions();
  // Enable both existential and universal preconditions.
  void EnableQuantifiedPreconditions();
  // Allow "when" in action effects.
  void EnableConditionalEffects();
  // Allow function definitions and use of effects using assignment operators
  // and arithmetic preconditions.
  void EnableFluents();
  // Enable typing, negative and disjunctive preconditions, equality, quantified
  // preconditions, and conditional effects.
  void EnableAdl();
  // Allow durative actions.  Note that this does not imply support for fluents.
  void EnableDurativeActions();
  // Allow duration constraints in durative actions using inequalities.
  void EnableDurationInequalities();
  // Allow durative actions to affect fluents continuously over the duration of
  // actions.
  void EnableContinuousEffects();
  // Allow the initial state to specify literals that will become true at a
  // specied time point.  Implies support for durative actions.
  void EnableTimedInitialLiterals();

  bool typing() const { return typing_; }
  bool negative_preconditions() const { return negative_preconditions_; }
  bool disjunctive_preconditions() const { return disjunctive_preconditions_; }
  bool equality() const { return equality_; }
  bool existential_preconditions() const { return existential_preconditions_; }
  bool universal_preconditions() const { return universal_preconditions_; }
  bool conditional_effects() const { return conditional_effects_; }
  bool fluents() const { return fluents_; }
  bool durative_actions() const { return durative_actions_; }
  bool duration_inequalities() const { return duration_inequalities_; }
  bool continuous_effects() const { return continuous_effects_; }
  bool timed_initial_literals() const { return timed_initial_literals_; }

 private:
  bool typing_ = false;
  bool negative_preconditions_ = false;
  bool disjunctive_preconditions_ = false;
  bool equality_ = false;
  bool existential_preconditions_ = false;
  bool universal_preconditions_ = false;
  bool conditional_effects_ = false;
  bool fluents_ = false;
  bool durative_actions_ = false;
  bool duration_inequalities_ = false;
  bool continuous_effects_ = false;
  bool timed_initial_literals_ = false;
};

#endif  // PDDL_REQUIREMENTS_H_
