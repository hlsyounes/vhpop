/* -*-C++-*- */
/*
 * Domain descriptions.
 *
 * $Id: domains.h,v 1.10 2001-08-16 00:39:01 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include <iostream>
#include <string>
#include <hash_map>
#include <hash_set>
#include <vector>
#include "support.h"
#include "types.h"


struct SubstitutionList;
struct TermList;
struct VariableList;
struct Name;
struct NameList;
struct NameMap;
struct Formula;
struct FormulaList;
struct AtomicFormula;

/*
 * Predicate declaration.
 */
struct Predicate : public gc {
  /* Name of this predicate. */
  const string name;
  /* Predicate parameters. */
  const VariableList& parameters;

  /* Constructs a predicate with the given names and parameters. */
  Predicate(const string& name, const VariableList& params)
    : name(name), parameters(params) {
  }

  /* Returns the arity of this predicate. */
  size_t arity() const;

private:
  /* Prints this predicate on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Predicate& p);
};

/* Output operator for predicates. */
inline ostream& operator<<(ostream& os, const Predicate& p) {
  p.print(os);
  return os;
}


/*
 * Table of predicate declarations.
 */
struct PredicateMap : public gc,
		      hash_map<string, const Predicate*, hash<string>,
		      equal_to<string>, container_alloc> {
};


/*
 * Effect definition.
 */
struct Effect : public gc {
  /* List of universally quantified variables for this effect. */
  const VariableList& forall;
  /* Condition for this effect, or NULL if unconditional effect. */
  const Formula* const condition;
  /* Add list for this effect. */
  const FormulaList& add_list;

  /* Constructs an unconditional single effect. */
  Effect(const Formula& add);

  /* Constructs an unconditional multiple effect. */
  Effect(const FormulaList& add_list);

  /* Constructs a conditional effect. */
  Effect(const Formula* condition, const FormulaList& add_list);

  /* Constructs a universally quantified conditional effect. */
  Effect(const VariableList& forall, const Formula* condition,
	 const FormulaList& add_list)
    : forall(forall), condition(condition), add_list(add_list) {
  }

  /* Returns an instantiation of this effect. */
  const Effect& instantiation(size_t id) const;

  /* Returns this effect subject to the given substitutions. */
  const Effect& substitution(const SubstitutionList& subst) const;

  /* Fills the provided list with goals achievable by this effect. */
  void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by the
     effect. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

private:
  /* Prints this effect on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Effect& e);
};

/* Output operator for effects. */
inline ostream& operator<<(ostream& os, const Effect& e) {
  e.print(os);
  return os;
}


/*
 * List of effect definitions.
 */
struct EffectList : public gc, vector<const Effect*, container_alloc> {
  /* Constructs an empty effect list. */
  EffectList() {
  }

  /* Constructs an effect list with a single effect. */
  EffectList(const Effect* effect)
    : vector<const Effect*, container_alloc>(1, effect) {
  }

  /* Returns an instantiation of this effect list. */
  const EffectList& instantiation(size_t id) const;

  /* Returns this effect list subject to the given substitutions. */
  const EffectList& substitution(const SubstitutionList& subst) const;

  /* Fills the provided list with goals achievable by the effect in
     this list. */
  void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by the effects
     in this list. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;
};


struct ActionList;
struct Problem;

/*
 * Abstract action definition.
 */
struct Action : public gc {
  /* Action precondition. */
  const Formula& precondition;
  /* List of action effects. */
  const EffectList& effects;
  /* Roughly corresponds to the number of open conditions this action
     will give rise to. */
  const size_t cost;

  /* Deletes this action. */
  virtual ~Action() {
  }

  /* Returns a formula representing this action. */
  virtual const AtomicFormula& action_formula(size_t id) const = 0;

  /* Fills the provided action list with all instantiations of this
     action. */
  virtual void instantiations(ActionList& actions,
			      const Problem& problem) const = 0;

  /* Fills the provided list with goals achievable by this action. */
  void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by this
     action. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

protected:
  /* Constructs an action. */
  Action(const Formula& precondition, const EffectList& effects);

  /* Prints this action on the given stream. */
  virtual void print(ostream& os) const = 0;

  /* Tests if this action equals the given action. */
  virtual bool equals(const Action& a) const = 0;

  /* Returns the hash value of this action. */
  virtual size_t hash_value() const = 0;

  friend ostream& operator<<(ostream& os, const Action& a);
  friend bool operator==(const Action& a1, const Action& a2);
  friend struct hash<Action>;
};

/* Output operator for actions. */
inline ostream& operator<<(ostream& os, const Action& a) {
  a.print(os);
  return os;
}

/* Equality operator for actions. */
inline bool operator==(const Action& a1, const Action& a2) {
  return a1.equals(a2);
}

/* Inequality operator for actions. */
inline bool operator!=(const Action& a1, const Action& a2) {
  return !(a1 == a2);
}

/*
 * Hash function object for actions.
 */
struct hash<Action> {
  size_t operator()(const Action& a) const {
    return a.hash_value();
  }
};

/*
 * Equality function object for action pointers.
 */
struct equal_to<const Action*> {
  bool operator()(const Action* a1, const Action* a2) const {
    return *a1 == *a2;
  }
};

/*
 * Hash function object for action pointers.
 */
struct hash<const Action*> {
  size_t operator()(const Action* a) const {
    return hash<Action>()(*a);
  }
};


/*
 * List of action definitions.
 */
struct ActionList : public gc, vector<const Action*, container_alloc> {
};


/*
 * Table of action definitions.
 */
struct ActionMap : public gc,
		   hash_map<string, const Action*, hash<string>,
		   equal_to<string>, container_alloc> {
};


/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Name of this action schema. */
  const string name;
  /* Action parameters. */
  const VariableList& parameters;

  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects)
    : Action(precondition, effects), name(name), parameters(parameters) {
  }

  /* Returns a formula representing this action. */
  virtual const AtomicFormula& action_formula(size_t id) const;

  /* Fills the provided action list with all instantiations of this
     action. */
  virtual void instantiations(ActionList& actions,
			      const Problem& problem) const;

protected:
  /* Prints this action on the given stream. */
  virtual void print(ostream& os) const;

  /* Tests if this action equals the given action. */
  virtual bool equals(const Action& a) const;

  /* Returns the hash value of this action. */
  virtual size_t hash_value() const;
};


/*
 * Ground action.
 */
struct GroundAction : public Action {
  /* Constructs a ground action, assuming arguments are names. */
  GroundAction(const string& name, const TermList& arguments,
	       const Formula& precondition, const EffectList& effects);

  /* Returns a formula representing this action. */
  virtual const AtomicFormula& action_formula(size_t id) const;

  /* Fills the provided action list with all instantiations of this
     action. */
  virtual void instantiations(ActionList& actions,
			      const Problem& problem) const;

protected:
  /* Prints this action on the given stream. */
  virtual void print(ostream& os) const;

  /* Tests if this action equals the given action. */
  virtual bool equals(const Action& a) const;

  /* Returns the hash value of this action. */
  virtual size_t hash_value() const;

private:
  /* Atomic formula representing this ground action. */
  const AtomicFormula& formula;
};


struct Requirements;

/*
 * Domain definition.
 */
struct Domain : public gc {
  /* Table of domain definitions. */
  typedef hash_map<string, const Domain*, hash<string>, equal_to<string>,
    container_alloc> DomainMap;

  /* Name of this domain. */
  const string name;
  /* Action style used for actions of this domain. */
  const Requirements& requirements;
  /* Domain types. */
  const TypeMap& types;
  /* Domain constants. */
  const NameMap& constants;
  /* Domain predicates. */
  const PredicateMap& predicates;
  /* Domain actions. */
  const ActionMap& actions;

  /* Returns a const_iterator pointing to the first domain. */
  static DomainMap::const_iterator begin() {
    return domains.begin();
  }

  /* Returns a const_iterator pointing beyond the last domain. */
  static DomainMap::const_iterator end() {
    return domains.end();
  }

  /* Returns the domain with the given name, or NULL it is undefined. */
  static const Domain* find(const string& name) {
    DomainMap::const_iterator i = domains.find(name);
    return (i != domains.end()) ? (*i).second : NULL;
  }

  /* Removes all defined domains. */
  static void clear() {
    domains.clear();
  }

  /* Constructs a domain. */
  Domain(const string& name, const Requirements& requirements,
	 const TypeMap& types, const NameMap& constants,
	 const PredicateMap& predicates, const ActionMap& actions)
    : name(name), requirements(requirements), types(types),
      constants(constants), predicates(predicates), actions(actions),
      static_predicates_(static_predicates(predicates, actions)) {
    domains[name] = this;
  }

  /* Deletes a domain. */
  ~Domain() {
    domains.erase(name);
  }

  /* Returns the type with the given name, or NULL if it is
     undefined. */
  const Type* find_type(const string& name) const;

  /* Returns the constant with the given name, or NULL if it is
     undefined. */
  const Name* find_constant(const string& name) const;

  /* Returns the predicate with the given name, or NULL if it is
     undefined. */
  const Predicate* find_predicate(const string& name) const;

  /* Returns the action with the given name, or NULL if it is
     undefined. */
  const Action* find_action(const string& name) const;

  /* Fills the provided name list with constants that are compatible
     with the given type. */
  void compatible_constants(NameList& constants,
			    const Type& t = SimpleType::OBJECT) const;

  /* Tests if the given predicate is static. */
  bool static_predicate(const string& predicate) const {
    return (static_predicates_.find(predicate) != static_predicates_.end());
  }

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Static predicates. */
  const hash_set<string> static_predicates_;

  /* Returns a set of static predicates. */
  static hash_set<string> static_predicates(const PredicateMap& predicates,
					    const ActionMap& actions);

  /* Prints this domain on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Domain& d);
};

/* Output operator for domains. */
inline ostream& operator<<(ostream& os, const Domain& d) {
  d.print(os);
  return os;
}

#endif /* DOMAINS_H */
