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
 * $Id: domains.h,v 3.8 2002-03-25 00:44:44 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include "support.h"
#include "requirements.h"
#include "types.h"
#include "formulas.h"
#include <hash_set>

struct Problem;


/* ====================================================================== */
/* Predicate */
/*
 * Predicate declaration.
 */
struct Predicate : public Printable {
  /* Name of this predicate. */
  const string name;

  /* Constructs a predicate with the given name. */
  Predicate(const string& name);

  /* Deletes this predicate. */
  virtual ~Predicate();

  /* Returns the arity of this predicate. */
  size_t arity() const;

  /* Returns the type of the ith parameter of this predicate. */
  const Type& type(size_t i) const;

  /* Adds a parameter to this predicate. */
  void add_parameter(const Type& type);

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* List of types. */
  struct TypeList : vector<const Type*> {
  };

  /* Iterator for type list. */
  typedef TypeList::const_iterator TypeListIter;

  /* Parameter types. */
  TypeList parameters_;
};


struct EffectList;

/*
 * Effect definition.
 */
struct Effect : public Printable {
  /* Possible temporal annotations for effects. */
  typedef enum { AT_START, AT_END } EffectTime;

  /* List of universally quantified variables for this effect. */
  const VariableList& forall;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Formula& condition;
  /* Add list for this effect. */
  const AtomList& add_list;
  /* Delete list for this effect. */
  const NegationList& del_list;
  /* Temporal annotation for this effect. */
  const EffectTime when;

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

  /* Returns an instantiation of this effect. */
  const Effect& instantiation(size_t id) const;

  /* Fills the provided list with instantiations of this effect. */
  void instantiations(EffectList& effects, const SubstitutionList& subst,
		      const Problem& problem) const;

  /* Returns this effect subject to the given substitutions. */
  const Effect& substitution(const SubstitutionList& subst) const;

  /* Fills the provided sets with predicates achievable by the
     effect. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * List of effect definitions.
 */
struct EffectList : public vector<const Effect*> {
  /* An empty effect list. */
  static const EffectList& EMPTY;

  /* Constructs an empty effect list. */
  EffectList();

  /* Constructs an effect list with a single effect. */
  EffectList(const Effect* effect);

  /* Returns an instantiation of this effect list. */
  const EffectList& instantiation(size_t id) const;

  /* Returns an instantiation of this effect list. */
  const EffectList& instantiation(const SubstitutionList& subst,
				  const Problem& problem) const;

  /* Returns this effect list subject to the given substitutions. */
  const EffectList& substitution(const SubstitutionList& subst) const;

  /* Fills the provided sets with predicates achievable by the effects
     in this list. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

  /* "Strengthens" this effect list. */
  const EffectList& stengthen(const Formula& condition) const;
};

/* Iterator for effect lists. */
typedef EffectList::const_iterator EffectListIter;


/*
 * Abstract action definition.
 */
struct Action : public Printable {
  /* Name of this action. */
  const string name;
  /* Action precondition. */
  const Formula& precondition;
  /* List of action effects. */
  const EffectList& effects;
  /* Whether this is a durative action. */
  bool durative;
  /* Minimum duration of this action. */
  float min_duration;
  /* Maximum duration of this action. */
  float max_duration;

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const = 0;

  /* Fills the provided sets with predicates achievable by this
     action. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

protected:
  /* Constructs an action. */
  Action(const string& name, const Formula& precondition,
	 const EffectList& effects);

  /* Constructs a durative action. */
  Action(const string& name, const Formula& precondition,
	 const EffectList& effects, float min_duration, float max_duration);
};


/*
 * List of action definitions.
 */
struct ActionList : public vector<const Action*> {
};

/* Iterator for action lists. */
typedef ActionList::const_iterator ActionListIter;


struct GroundActionList;
struct Domain;

/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Action schema parameters. */
  const VariableList& parameters;

  /* Constructs an action schema. */
  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects);

  /* Constructs an action schema for a durative action. */
  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects,
	       float min_duration, float max_duration);

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const;

  /* Fills the provided action list with all instantiations of this
     action schema. */
  void instantiations(GroundActionList& actions, const Problem& problem) const;

  /* Returns this action schema with all static preconditions assumed
     true. */
  const ActionSchema& strip_static(const Domain& domain) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Table of action schema definitions.
 */
struct ActionSchemaMap : public hash_map<string, const ActionSchema*> {
};

/* Iterator for action schema table. */
typedef ActionSchemaMap::const_iterator ActionSchemaMapIter;


/*
 * Ground action.
 */
struct GroundAction : public Action {
  /* Action arguments. */
  const NameList& arguments;

  /* Constructs a ground action, assuming arguments are names. */
  GroundAction(const string& name, const NameList& arguments,
	       const Formula& precondition, const EffectList& effects);

  /* Constructs a ground durative action, assuming arguments are names. */
  GroundAction(const string& name, const NameList& arguments,
	       const Formula& precondition, const EffectList& effects,
	       float min_duration, float max_duration);

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Atomic representation of this ground action. */
  mutable const Atom* formula_;
};


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
struct TypeMap : hash_map<string, const SimpleType*> {
};

/* Iterator for type tables. */
typedef TypeMap::const_iterator TypeMapIter;


/*
 * Domain definition.
 */
struct Domain : public Printable {
  /* Table of domain definitions. */
  struct DomainMap : public hash_map<string, const Domain*> {
  };

  /* Iterator for domain tables. */
  typedef DomainMap::const_iterator DomainMapIter;

  /* Name of this domain. */
  const string name;
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
  virtual ~Domain();

  /* Domain actions. */
  const ActionSchemaMap& actions() const;

  /* Adds a type to this domain. */
  void add_type(const SimpleType& type);

  /* Adds a constant to this domain. */
  void add_constant(const Name& constant);

  /* Adds a predicate to this domain. */
  void add_predicate(const Predicate& predicate);

  /* Adds an action to this domain. */
  void add_action(const ActionSchema& action);

  /* Returns the type with the given name, or NULL if it is
     undefined. */
  const SimpleType* find_type(const string& name) const;

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
  bool static_predicate(const string& predicate) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Table of predicate declarations. */
  struct PredicateMap : public hash_map<string, const Predicate*> {
  };

  /* Iterator for predicate table. */
  typedef PredicateMap::const_iterator PredicateMapIter;

  /* Table of all defined domains. */
  static DomainMap domains;

  /* Domain types. */
  TypeMap types_;
  /* Domain constants. */
  NameMap constants_;
  /* Domain predicates. */
  PredicateMap predicates_;
  /* Domain action schemas. */
  ActionSchemaMap actions_;
  /* Static predicates. */
  hash_set<string> static_predicates_;
};


#endif /* DOMAINS_H */
