/* -*-C++-*- */
/*
 * Domain descriptions.
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
 * $Id: domains.h,v 4.6 2002-11-05 04:41:53 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include "support.h"
#include "requirements.h"
#include "types.h"
#include "formulas.h"
#include <map>

struct Problem;


/* ====================================================================== */
/* Predicate */
/*
 * Predicate declaration.
 */
struct Predicate {
  /* Constructs a predicate with the given name. */
  explicit Predicate(const string& name);

  /* Deletes this predicate. */
  ~Predicate();

  /* Returns the name of this predicate. */
  const string& name() const { return name_; }

  /* Returns the arity of this predicate. */
  size_t arity() const;

  /* Returns the type of the ith parameter of this predicate. */
  const Type& type(size_t i) const;

  /* Adds a parameter to this predicate. */
  void add_parameter(const Type& type);

private:
  /* List of types. */
  struct TypeList : vector<const Type*> {
  };

  /* Iterator for type list. */
  typedef TypeList::const_iterator TypeListIter;

  /* Name of this predicate. */
  string name_;
  /* Parameter types. */
  TypeList parameters_;
};

/*
 * Hash function object for predicates.
 */
namespace __gnu_cxx {
struct hash<const Predicate*> {
  size_t operator()(const Predicate* p) const {
    return size_t(p);
  }
};
}

/* Equality operator for predicates. */
inline bool operator==(const Predicate& p1, const Predicate& p2) {
  return &p1 == &p2;
}

/* Inequality operator for predicates. */
inline bool operator!=(const Predicate& p1, const Predicate& p2) {
  return &p1 != &p2;
}

/* Output operator for predicates. */
ostream& operator<<(ostream& os, const Predicate& p);


/* ====================================================================== */
/* PredicateMap */

/* Table of predicate declarations. */
struct PredicateMap : public hash_map<string, const Predicate*> {
};

/* Iterator for predicate table. */
typedef PredicateMap::const_iterator PredicateMapIter;


/* ====================================================================== */
/* PredicateSet */

/* Set of predicate declarations. */
struct PredicateSet : public hash_set<const Predicate*> {
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

  /* Constructs an unconditional effect. */
  Effect(const AtomList& add_list, const NegationList& del_list,
	 EffectTime when);

  /* Constructs a conditional effect. */
  Effect(const Formula& condition,
	 const AtomList& add_list, const NegationList& del_list,
	 EffectTime when);

  /* Constructs a universally quantified unconditional effect. */
  Effect(const VariableList& forall,
	 const AtomList& add_list, const NegationList& del_list,
	 EffectTime when);

  /* Constructs a universally quantified conditional effect. */
  Effect(const VariableList& forall, const Formula& condition,
	 const AtomList& add_list, const NegationList& del_list,
	 EffectTime when);

  /* Returns the universally quantified variables for this effect. */
  const VariableList& forall() const { return *forall_; }

  /* Returns the condition for this effect. */
  const Formula& condition() const { return *condition_; }

  /* Returns the condition that must hold for this effect to be
     considered for linking. */
  const Formula& link_condition() const { return *link_condition_; }

  /* Returns the add list for this effect. */
  const AtomList& add_list() const { return *add_list_; }

  /* Returns the delete list for this effect. */
  const NegationList& del_list() const { return *del_list_; }

  /* Returns the temporal annotation for this effect. */
  EffectTime when() const { return when_; }

  /* Fills the provided list with instantiations of this effect. */
  void instantiations(EffectList& effects, const SubstitutionList& subst,
		      const Problem& problem) const;

  /* Fills the provided sets with predicates achievable by the
     effect. */
  void achievable_predicates(PredicateSet& preds,
			     PredicateSet& neg_preds) const;

  /* Returns a copy of this effect with a new link condition. */
  const Effect& new_link_condition(const Formula& cond) const;

private:
  /* Constructs a universally quantified conditional effect with a
     link condition. */
  Effect(const VariableList& forall, const Formula& condition,
	 const AtomList& add_list, const NegationList& del_list,
	 EffectTime when, const Formula& link_cond);

  /* List of universally quantified variables for this effect. */
  const VariableList* forall_;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Formula* condition_;
  /* Condition that must hold for this effect to be considered for linking. */
  const Formula* link_condition_;
  /* Add list for this effect. */
  const AtomList* add_list_;
  /* Delete list for this effect. */
  const NegationList* del_list_;
  /* Temporal annotation for this effect. */
  EffectTime when_;
};

/* Output operator for effects. */
ostream& operator<<(ostream& os, const Effect& e);


/* ====================================================================== */
/* EffectList */

/*
 * List of effect definitions.
 */
struct EffectList : public vector<const Effect*> {
  /* An empty effect list. */
  static const EffectList EMPTY;

  /* Constructs an empty effect list. */
  EffectList();

  /* Constructs an effect list with a single effect. */
  EffectList(const Effect* effect);

  /* Returns an instantiation of this effect list. */
  const EffectList& instantiation(const SubstitutionList& subst,
				  const Problem& problem) const;

  /* Fills the provided sets with predicates achievable by the effects
     in this list. */
  void achievable_predicates(PredicateSet& preds,
			     PredicateSet& neg_preds) const;

  /* "Strengthens" this effect list. */
  const EffectList& strengthen(const Formula& condition) const;
};

/* Iterator for effect lists. */
typedef EffectList::const_iterator EffectListIter;


/* ====================================================================== */
/* Action */

/*
 * Abstract action definition.
 */
struct Action {
  /* Returns the name of this action. */
  const string& name() const { return name_; }

  /* Return the precondition of this action. */
  const Formula& precondition() const { return *precondition_; }

  /* List of action effects. */
  const EffectList& effects() const { return *effects_; }

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

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings* bindings) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const = 0;

protected:
  /* Constructs an action. */
  Action(const string& name, const Formula& precondition,
	 const EffectList& effects);

  /* Constructs a durative action. */
  Action(const string& name, const Formula& precondition,
	 const EffectList& effects, float min_duration, float max_duration);

private:
  /* Name of this action. */
  string name_;
  /* Action precondition. */
  const Formula* precondition_;
  /* List of action effects. */
  const EffectList* effects_;
  /* Whether this is a durative action. */
  bool durative_;
  /* Minimum duration of this action. */
  float min_duration_;
  /* Maximum duration of this action. */
  float max_duration_;

  friend ostream& operator<<(ostream& os, const Action& a);
};

/* Output operator for actions. */
ostream& operator<<(ostream& os, const Action& a);


/* ====================================================================== */
/* ActionList */

/*
 * List of action definitions.
 */
struct ActionList : public vector<const Action*> {
};

/* Iterator for action lists. */
typedef ActionList::const_iterator ActionListIter;


/* ====================================================================== */
/* ActionSchema */

struct GroundActionList;
struct Domain;

/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Constructs an action schema. */
  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects);

  /* Constructs an action schema for a durative action. */
  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects,
	       float min_duration, float max_duration);

  /* Returns the parameters of this action schema. */
  const VariableList& parameters() const { return *parameters_; }

  /* Fills the provided action list with all instantiations of this
     action schema. */
  void instantiations(GroundActionList& actions, const Problem& problem) const;

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings* bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Action schema parameters. */
  const VariableList* parameters_;
};


/* ====================================================================== */
/* ActionSchemaMap */

/*
 * Table of action schema definitions.
 */
struct ActionSchemaMap : public hash_map<string, const ActionSchema*> {
};

/* Iterator for action schema table. */
typedef ActionSchemaMap::const_iterator ActionSchemaMapIter;


/* ====================================================================== */
/* GroundAction */

/*
 * Ground action.
 */
struct GroundAction : public Action {
  /* Constructs a ground action, assuming arguments are names. */
  GroundAction(const string& name, const NameList& arguments,
	       const Formula& precondition, const EffectList& effects);

  /* Constructs a ground durative action, assuming arguments are names. */
  GroundAction(const string& name, const NameList& arguments,
	       const Formula& precondition, const EffectList& effects,
	       float min_duration, float max_duration);

  /* Action arguments. */
  const NameList& arguments() const { return *arguments_; }

  /* Prints this action on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings* bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Action arguments. */
  const NameList* arguments_;
};


/* ====================================================================== */
/* GroundActionList */

/*
 * List of ground actions.
 */
struct GroundActionList : public vector<const GroundAction*> {
};

/* Iterator for ground action lists. */
typedef GroundActionList::const_iterator GroundActionListIter;


/* ====================================================================== */
/* TypeMap */

/*
 * Table of simple types.
 */
struct TypeMap : map<string, SimpleType*> {
};

/* Iterator for type tables. */
typedef TypeMap::const_iterator TypeMapIter;


/* ====================================================================== */
/* Domain */

/*
 * Domain definition.
 */
struct Domain {
  /* Table of domain definitions. */
  struct DomainMap : public hash_map<string, const Domain*> {
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
  static const Domain* find(const string& name);

  /* Removes all defined domains. */
  static void clear();

  /* Constructs an empty domain with the given name. */
  Domain(const string& name);

  /* Deletes a domain. */
  ~Domain();

  /* Returns the name of this domain. */
  const string& name() const { return name_; }

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
  SimpleType* find_type(const string& name);

  /* Returns the type with the given name, or NULL if it is
     undefined. */
  const SimpleType* find_type(const string& name) const;

  /* Returns the constant with the given name, or NULL if it is
     undefined. */
  Name* find_constant(const string& name);

  /* Returns the constant with the given name, or NULL if it is
     undefined. */
  const Name* find_constant(const string& name) const;

  /* Returns the predicate with the given name, or NULL if it is
     undefined. */
  const Predicate* find_predicate(const string& name) const;

  /* Returns the action schema with the given name, or NULL if it is
     undefined. */
  const ActionSchema* find_action(const string& name) const;

  /* Fills the provided name list with constants that are compatible
     with the given type. */
  void compatible_constants(NameList& constants, const Type& t) const;

  /* Tests if the given predicate is static. */
  bool static_predicate(const Predicate& predicate) const;

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Name of this domain. */
  string name_;
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

  friend ostream& operator<<(ostream& os, const Domain& d);
};

/* Output operator for domains. */
ostream& operator<<(ostream& os, const Domain& d);


#endif /* DOMAINS_H */
