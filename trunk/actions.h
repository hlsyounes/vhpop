/* -*-C++-*- */
/*
 * Actions.
 *
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: actions.h,v 6.1 2003-08-24 22:11:32 lorens Exp $
 */
#ifndef ACTIONS_H
#define ACTIONS_H

#include <config.h>
#include "effects.h"


/* ====================================================================== */
/* Action */

/*
 * Abstract action definition.
 */
struct Action {
  /* Deletes this action. */
  virtual ~Action();

  /* Sets the condition for this action. */
  void set_condition(const Condition& condition);

  /* Adds an effect to this action. */
  void add_effect(const Effect& effect);

  /* Sets the minimum duration for this action. */
  void set_min_duration(double min_duration);

  /* Sets the maximum duration for this action. */
  void set_max_duration(double max_duration);

  /* Sets the duration for this action. */
  void set_duration(double duration);

  /* Returns the name of this action. */
  const std::string& name() const { return name_; }

  /* Return the condition of this action. */
  const Condition& condition() const { return *condition_; }

  /* List of action effects. */
  const EffectList& effects() const { return effects_; }

  /* Whether this is a durative action. */
  bool durative() const { return durative_; }

  /* Minimum duration of this action. */
  float min_duration() const { return min_duration_; }

  /* Maximum duration of this action. */
  float max_duration() const { return max_duration_; }

  /* "Strengthens" the effects of this action. */
  void strengthen_effects();

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const TermTable& terms,
		     size_t step_id, const Bindings& bindings) const = 0;

protected:
  /* Constructs an action with the given name. */
  Action(const std::string& name, bool durative);

private:
  /* Name of this action. */
  std::string name_;
  /* Action condition. */
  const Condition* condition_;
  /* List of action effects. */
  EffectList effects_;
  /* Whether this is a durative action. */
  bool durative_;
  /* Minimum duration of this action. */
  float min_duration_;
  /* Maximum duration of this action. */
  float max_duration_;
};


/* ====================================================================== */
/* ActionList */

/*
 * List of action definitions.
 */
struct ActionList : public std::vector<const Action*> {
};


/* ====================================================================== */
/* ActionSchema */

struct GroundAction;
struct GroundActionList;

/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Constructs an action schema with the given name. */
  ActionSchema(const std::string& name, bool durative);

  /* Adds a parameter to this action schema. */
  void add_parameter(Variable var);

  /* Returns the parameters of this action schema. */
  const VariableList& parameters() const { return parameters_; }

  /* Fills the provided action list with all instantiations of this
     action schema. */
  void instantiations(GroundActionList& actions, const Problem& problem) const;

  /* Prints this action on the given stream. */
  void print(std::ostream& os, const PredicateTable& predicates,
	     const TermTable& terms) const;

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const TermTable& terms,
		     size_t step_id, const Bindings& bindings) const;

private:
  /* Action schema parameters. */
  VariableList parameters_;

  /* Returns an instantiation of this action schema. */
  const GroundAction* instantiation(const SubstitutionMap& args,
				    const Problem& problem,
				    const Condition& condition) const;
};


/* ====================================================================== */
/* ActionSchemaMap */

/*
 * Table of action schema definitions.
 */
struct ActionSchemaMap : public std::map<std::string, const ActionSchema*> {
};

/* Iterator for action schema table. */
typedef ActionSchemaMap::const_iterator ActionSchemaMapIter;


/* ====================================================================== */
/* GroundAction */

/*
 * Ground action.
 */
struct GroundAction : public Action {
  /* Constructs a ground action with the given name. */
  GroundAction(const std::string& name, bool durative);

  /* Adds an argument to this ground action. */
  void add_argument(Object arg);

  /* Action arguments. */
  const ObjectList& arguments() const { return arguments_; }

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const TermTable& terms,
		     size_t step_id, const Bindings& bindings) const;

private:
  /* Action arguments. */
  ObjectList arguments_;
};


/* ====================================================================== */
/* GroundActionList */

/*
 * List of ground actions.
 */
struct GroundActionList : public std::vector<const GroundAction*> {
};

/* Iterator for ground action lists. */
typedef GroundActionList::const_iterator GroundActionListIter;


#endif /* ACTIONS_H */
