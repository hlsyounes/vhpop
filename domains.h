/* -*-C++-*- */
/*
 * Domain descriptions.
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
 * $Id: domains.h,v 4.11 2003-03-01 18:52:45 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include <config.h>
#include "requirements.h"
#include "formulas.h"
#include "types.h"
#include "hashing.h"
#include <map>

struct Problem;


/* ====================================================================== */
/* Predicate */
/*
 * Predicate declaration.
 */
struct Predicate {
  /* Constructs a predicate with the given name. */
  explicit Predicate(const std::string& name);

  /* Deletes this predicate. */
  ~Predicate();

  /* Returns the name of this predicate. */
  const std::string& name() const { return name_; }

  /* Returns the arity of this predicate. */
  size_t arity() const;

  /* Returns the type of the ith parameter of this predicate. */
  const Type& type(size_t i) const;

  /* Adds a parameter to this predicate. */
  void add_parameter(const Type& type);

private:
  /* Name of this predicate. */
  std::string name_;
  /* Parameter types. */
  TypeList parameters_;
};

/* Equality operator for predicates. */
inline bool operator==(const Predicate& p1, const Predicate& p2) {
  return &p1 == &p2;
}

/* Inequality operator for predicates. */
inline bool operator!=(const Predicate& p1, const Predicate& p2) {
  return &p1 != &p2;
}

/* Output operator for predicates. */
std::ostream& operator<<(std::ostream& os, const Predicate& p);


/* ====================================================================== */
/* PredicateMap */

/* Table of predicate declarations. */
struct PredicateMap : public std::map<std::string, const Predicate*> {
};

/* Iterator for predicate table. */
typedef PredicateMap::const_iterator PredicateMapIter;


/* ====================================================================== */
/* PredicateSet */

/* Set of predicate declarations. */
struct PredicateSet : public hashing::hash_set<const Predicate*> {
};

/* Iterator for predicate sets. */
typedef PredicateSet::const_iterator PredicateSetIter;


/* ====================================================================== */
/* Effect */

struct EffectList;

/*
 * Effect definition.
 */
struct Effect {
  /* Possible temporal annotations for effects. */
  typedef enum { AT_START, AT_END } EffectTime;

  /* Constructs an empty effect. */
  explicit Effect(EffectTime when);

  /* Deletes this effect. */
  ~Effect();

  /* Adds a universally quantified variable to this effect. */
  void add_forall(const Variable& parameter);

  /* Sets the condition of this effect. */
  void set_condition(const Formula& condition);

  /* Sets the link condition of this effect. */
  void set_link_condition(const Formula& link_condition) const;

  /* Adds an atom to the add list of this effect. */
  void add_positive(const Atom& atom);

  /* Adds a negated atom to the delete list of this effect. */
  void add_negative(const Negation& negation);

  /* Returns the universally quantified variables for this effect. */
  const VariableList& forall() const { return forall_; }

  /* Returns the condition for this effect. */
  const Formula& condition() const { return *condition_; }

  /* Returns the condition that must hold for this effect to be
     considered for linking. */
  const Formula& link_condition() const { return *link_condition_; }

  /* Returns the add list for this effect. */
  const AtomList& add_list() const { return add_list_; }

  /* Returns the delete list for this effect. */
  const NegationList& del_list() const { return del_list_; }

  /* Returns the temporal annotation for this effect. */
  EffectTime when() const { return when_; }

  /* Fills the provided list with instantiations of this effect. */
  void instantiations(EffectList& effects, size_t& useful,
		      const SubstitutionList& subst,
		      const Problem& problem) const;

  /* Fills the provided sets with predicates achievable by the
     effect. */
  void achievable_predicates(PredicateSet& preds,
			     PredicateSet& neg_preds) const;

private:
  /* List of universally quantified variables for this effect. */
  VariableList forall_;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Formula* condition_;
  /* Condition that must hold for this effect to be considered for linking. */
  mutable const Formula* link_condition_;
  /* Add list for this effect. */
  AtomList add_list_;
  /* Delete list for this effect. */
  NegationList del_list_;
  /* Temporal annotation for this effect. */
  EffectTime when_;

  /* Returns an instantiation of this effect. */
  const Effect* instantiation(const SubstitutionList& args,
			      const Problem& problem,
			      const Formula& condition) const;
};

/* Output operator for effects. */
std::ostream& operator<<(std::ostream& os, const Effect& e);


/* ====================================================================== */
/* EffectList */

/*
 * List of effect definitions.
 */
struct EffectList : public std::vector<const Effect*> {
};

/* Iterator for effect lists. */
typedef EffectList::const_iterator EffectListIter;


/* ====================================================================== */
/* Action */

/*
 * Abstract action definition.
 */
struct Action {
  /* Deletes this action. */
  virtual ~Action();

  /* Sets the precondition for this action. */
  void set_precondition(const Formula& precondition);

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

  /* Return the precondition of this action. */
  const Formula& precondition() const { return *precondition_; }

  /* List of action effects. */
  const EffectList& effects() const { return effects_; }

  /* Whether this is a durative action. */
  bool durative() const { return durative_; }

  /* Minimum duration of this action. */
  float min_duration() const { return min_duration_; }

  /* Maximum duration of this action. */
  float max_duration() const { return max_duration_; }

  /* Fills the provided sets with predicates achievable by this
     action. */
  void achievable_predicates(PredicateSet& preds,
			     PredicateSet& neg_preds) const;

  /* "Strengthens" the effects of this action. */
  void strengthen_effects();

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, size_t step_id,
		     const Bindings* bindings) const = 0;

protected:
  /* Constructs an action with the given name. */
  Action(const std::string& name, bool durative);

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Name of this action. */
  std::string name_;
  /* Action precondition. */
  const Formula* precondition_;
  /* List of action effects. */
  EffectList effects_;
  /* Whether this is a durative action. */
  bool durative_;
  /* Minimum duration of this action. */
  float min_duration_;
  /* Maximum duration of this action. */
  float max_duration_;

  friend std::ostream& operator<<(std::ostream& os, const Action& a);
};

/* Output operator for actions. */
std::ostream& operator<<(std::ostream& os, const Action& a);


/* ====================================================================== */
/* ActionList */

/*
 * List of action definitions.
 */
struct ActionList : public std::vector<const Action*> {
};

/* Iterator for action lists. */
typedef ActionList::const_iterator ActionListIter;


/* ====================================================================== */
/* ActionSchema */

struct GroundAction;
struct GroundActionList;
struct Domain;

/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Constructs an action schema with the given name. */
  ActionSchema(const std::string& name, bool durative);

  /* Deletes this action schema. */
  virtual ~ActionSchema();

  /* Adds a parameter to this action schema. */
  void add_parameter(const Variable& var);

  /* Returns the parameters of this action schema. */
  const VariableList& parameters() const { return parameters_; }

  /* Fills the provided action list with all instantiations of this
     action schema. */
  void instantiations(GroundActionList& actions, const Problem& problem) const;

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, size_t step_id,
		     const Bindings* bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Action schema parameters. */
  VariableList parameters_;

  /* Returns an instantiation of this action schema. */
  const GroundAction* instantiation(const SubstitutionList& args,
				    const Problem& problem,
				    const Formula& precond) const;
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
  void add_argument(const Name& arg);

  /* Action arguments. */
  const NameList& arguments() const { return arguments_; }

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(std::ostream& os, size_t step_id,
		     const Bindings* bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Action arguments. */
  NameList arguments_;
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


/* ====================================================================== */
/* Domain */

/*
 * Domain definition.
 */
struct Domain {
  /* Table of domain definitions. */
  struct DomainMap : public std::map<std::string, const Domain*> {
  };

  /* Iterator for domain tables. */
  typedef DomainMap::const_iterator DomainMapIter;

  /* Requirements for this domain. */
  Requirements requirements;

  /* Returns a const_iterator pointing to the first domain. */
  static DomainMapIter begin();

  /* Returns a const_iterator pointing beyond the last domain. */
  static DomainMapIter end();

  /* Returns the domain with the given name, or NULL it is undefined. */
  static const Domain* find(const std::string& name);

  /* Removes all defined domains. */
  static void clear();

  /* Constructs an empty domain with the given name. */
  Domain(const std::string& name);

  /* Deletes a domain. */
  ~Domain();

  /* Returns the name of this domain. */
  const std::string& name() const { return name_; }

  /* Domain actions. */
  const ActionSchemaMap& actions() const;

  /* Adds a type to this domain. */
  void add_type(SimpleType& type);

  /* Adds a constant to this domain. */
  void add_constant(Name& constant);

  /* Adds a predicate to this domain. */
  void add_predicate(const Predicate& predicate);

  /* Adds an action to this domain. */
  void add_action(const ActionSchema& action);

  /* Returns the type with the given name, or NULL if it is
     undefined. */
  SimpleType* find_type(const std::string& name);

  /* Returns the type with the given name, or NULL if it is
     undefined. */
  const SimpleType* find_type(const std::string& name) const;

  /* Returns the constant with the given name, or NULL if it is
     undefined. */
  Name* find_constant(const std::string& name);

  /* Returns the constant with the given name, or NULL if it is
     undefined. */
  const Name* find_constant(const std::string& name) const;

  /* Returns the predicate with the given name, or NULL if it is
     undefined. */
  const Predicate* find_predicate(const std::string& name) const;

  /* Returns the action schema with the given name, or NULL if it is
     undefined. */
  const ActionSchema* find_action(const std::string& name) const;

  /* Fills the provided name list with constants that are compatible
     with the given type. */
  void compatible_constants(NameList& constants, const Type& t) const;

  /* Tests if the given predicate is static. */
  bool static_predicate(const Predicate& predicate) const;

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Name of this domain. */
  std::string name_;
  /* Domain types. */
  TypeMap types_;
  /* Domain constants. */
  NameMap constants_;
  /* Domain predicates. */
  PredicateMap predicates_;
  /* Domain action schemas. */
  ActionSchemaMap actions_;
  /* Static predicates. */
  PredicateSet static_predicates_;

  friend std::ostream& operator<<(std::ostream& os, const Domain& d);
};

/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d);


#endif /* DOMAINS_H */
