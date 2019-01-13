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
// Actions.

#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "bindings.h"
#include "effects.h"
#include "expressions.h"
#include "terms.h"

struct Domain;

// Abstract action definition.
class Action {
 public:
  // Deletes this action.
  virtual ~Action();

  // Sets the condition for this action.
  void set_condition(const Formula& condition);

  // Adds an effect to this action.
  void add_effect(const Effect& effect);

  // Sets the minimum duration for this action.
  void set_min_duration(const Expression& min_duration);

  // Sets the maximum duration for this action.
  void set_max_duration(const Expression& max_duration);

  // Sets the duration for this action.
  void set_duration(const Expression& duration);

  // Returns the id for this action.
  size_t id() const { return id_; }

  // Returns the name of this action.
  const std::string& name() const { return name_; }

  // Return the condition of this action.
  const Formula& condition() const { return *condition_; }

  // List of action effects.
  const EffectList& effects() const { return effects_; }

  // Whether this is a durative action.
  bool durative() const { return durative_; }

  // Minimum duration of this action.
  const Expression& min_duration() const { return *min_duration_; }

  // Maximum duration of this action.
  const Expression& max_duration() const { return *max_duration_; }

  // "Strengthens" the effects of this action.
  void strengthen_effects(const Domain& domain);

  // Prints this action on the given stream with the given bindings.
  virtual void print(std::ostream& os, size_t step_id,
                     const Bindings& bindings) const = 0;

 protected:
  // Constructs an action with the given name.
  Action(const std::string& name, bool durative);

 private:
  // Next action id.
  static size_t next_id;

  // Unique id for actions.
  size_t id_;
  // Name of this action.
  std::string name_;
  // Action condition.
  const Formula* condition_;
  // List of action effects.
  EffectList effects_;
  // Whether this is a durative action.
  bool durative_;
  // Minimum duration of this action.
  const Expression* min_duration_;
  // Maximum duration of this action.
  const Expression* max_duration_;
};

// Ground action.
class GroundAction : public Action {
 public:
  // Constructs a ground action with the given name.
  GroundAction(const std::string& name, bool durative);

  // Adds an argument to this ground action.
  void add_argument(Object arg);

  // Action arguments.
  const std::vector<Object>& arguments() const { return arguments_; }

  // Prints this action on the given stream with the given bindings.
  virtual void print(std::ostream& os, size_t step_id,
                     const Bindings& bindings) const;

 private:
  // Action arguments.
  std::vector<Object> arguments_;
};

// Action schema definition.
class ActionSchema : public Action {
 public:
  // Constructs an action schema with the given name.
  ActionSchema(const std::string& name, bool durative);

  // Adds a parameter to this action schema.
  void add_parameter(Variable var);

  // Returns the parameters of this action schema.
  const std::vector<Variable>& parameters() const { return parameters_; }

  // Fills the provided action list with all instantiations of this action
  // schema.
  void instantiations(std::vector<const GroundAction*>& actions,
                      const Problem& problem) const;

  // Prints this action on the given stream.
  void print(std::ostream& os) const;

  // Prints this action on the given stream with the given bindings.
  virtual void print(std::ostream& os, size_t step_id,
                     const Bindings& bindings) const;

 private:
  // Action schema parameters.
  std::vector<Variable> parameters_;

  // Returns an instantiation of this action schema.
  const GroundAction* instantiation(const std::map<Variable, Term>& args,
                                    const Problem& problem,
                                    const Formula& condition) const;
};

#endif  // ACTIONS_H_
