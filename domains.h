/* -*-C++-*- */
/*
 * Domain descriptions.
 *
 * $Id: domains.h,v 1.4 2001-05-11 22:11:28 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include <iostream>
#include <string>
#include <hash_map>
#include <hash_set>
#include <vector>
#include "support.h"
#include "formulas.h"


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
  unsigned int arity() const {
    return parameters.size();
  }

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
typedef hash_map<string, const Predicate*, hash<string>, equal_to<string>,
  container_alloc> PredicateMap;


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
  Effect(const Formula& add)
    : forall(*(new VariableList())), condition(NULL),
      add_list(*(new FormulaList(&add))) {
  }

  /* Constructs an unconditional multiple effect. */
  Effect(const FormulaList& add_list)
    : forall(*(new VariableList())), condition(NULL), add_list(add_list) {
  }

  /* Constructs a conditional effect. */
  Effect(const Formula* condition, const FormulaList& add_list)
    : forall(*(new VariableList())), condition(condition), add_list(add_list) {
  }

  /* Constructs a universally quantified conditional effect. */
  Effect(const VariableList& forall, const Formula* condition,
	 const FormulaList& add_list)
    : forall(forall), condition(condition), add_list(add_list) {
  }

  /* Returns an instantiation of this effect. */
  const Effect& instantiation(unsigned int id) const {
    const Formula* inst_condition =
      (condition != NULL) ? &condition->instantiation(id) : NULL;
    return *(new Effect(forall.instantiation(id), inst_condition,
			add_list.instantiation(id)));
  }

  /* Returns an effect subject to the given substitutions. */
  const Effect& substitution(const SubstitutionList& subst) const {
    SubstitutionList eff_subst;
    if (forall.empty()) {
      eff_subst = subst;
    } else {
      for (SubstitutionList::const_iterator i = subst.begin();
	   i != subst.end(); i++) {
	const Substitution& s = *i;
	if (!forall.contains(*s.first)) {
	  eff_subst.push_back(s);
	}
      }
    }
    const Formula* subst_condition =
      (condition != NULL) ? &condition->substitution(eff_subst) : NULL;
    return *(new Effect(forall, subst_condition,
			add_list.substitution(eff_subst)));
  }

  /* Checks if the add list of this effect matches the given formula. */
  bool matches(const Formula& f) const {
    return add_list.matches(f);
  }

  /* Checks if the add list of this effect involves the given predicate. */
  bool involves(const string& predicate) const {
    return add_list.involves(predicate);
  }

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
struct EffectList : public gc,
		    public vector<const Effect*, container_alloc> {
  /* Constructs an empty effect list. */
  EffectList() {
  }

  /* Constructs an effect list with a single effect. */
  EffectList(const Effect* effect)
    : vector<const Effect*, container_alloc>(1, effect) {
  }

  /* Returns an instantiation of this effect list. */
  const EffectList& instantiation(unsigned int id) const {
    EffectList& effects = *(new EffectList());
    for (const_iterator i = begin(); i != end(); i++) {
      effects.push_back(&(*i)->instantiation(id));
    }
    return effects;
  }

  /* Returns this effect list subject to the given substitutions. */
  const EffectList& substitution(const SubstitutionList& subst) const {
    EffectList& effects = *(new EffectList());
    for (const_iterator i = begin(); i != end(); i++) {
      effects.push_back(&(*i)->substitution(subst));
    }
    return effects;
  }

  /* Checks if any of the effects in this list matches the given formula. */
  bool matches(const Formula& f) const {
    for (const_iterator i = begin(); i != end(); i++) {
      if ((*i)->matches(f)) {
	return true;
      }
    }
    return false;
  }

  /* Checks if any of the effects in this list involves the given predicate. */
  bool involves(const string& predicate) const {
    for (const_iterator i = begin(); i != end(); i++) {
      if ((*i)->involves(predicate)) {
	return true;
      }
    }
    return false;
  }
};


/*
 * Action definition.
 */
struct Action : public gc {
  /* Name of this action. */
  const string name;
  /* Action parameters. */
  const VariableList& parameters;
  /* Action precondition, or NULL is this action lacks preconditions. */
  const Formula* const precondition;
  /* List of action effects. */
  const EffectList& effects;

  /* Constructs an action. */
  Action(const string& name, const VariableList& parameters,
	 const Formula* precondition, const EffectList& effects)
    : name(name), parameters(parameters), precondition(precondition),
      effects(effects) {
  }

  /* Checks if any effect of this action can achive the given goal. */
  bool applicable(const Formula& goal) const {
    return effects.matches(goal);
  }

private:
  /* Prints this action on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Action& a);
};

/* Output operator for actions. */
inline ostream& operator<<(ostream& os, const Action& a) {
  a.print(os);
  return os;
}


/*
 * List of action definitions.
 */
typedef vector<const Action*, container_alloc> ActionList;


/*
 * Table of action definitions.
 */
typedef hash_map<string, const Action*, hash<string>, equal_to<string>,
  container_alloc> ActionMap;


/*
 * Domain definition.
 */
struct Domain : public gc {
  /* Table of domain definitions. */
  typedef hash_map<string, const Domain*, hash<string>, equal_to<string>,
    container_alloc> DomainMap;
  /* Action styles. */
  typedef enum { STRIPS, ADL } ActionStyle;

  /* Name of this domain. */
  const string name;
  /* Action style used for actions of this domain. */
  const ActionStyle action_style;

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
  Domain(const string& name, ActionStyle action_style,
	 const TypeMap& types, const NameMap& constants,
	 const PredicateMap& predicates, const ActionMap& actions)
    : name(name), action_style(action_style), types_(types),
      constants_(constants), predicates_(predicates), actions_(actions),
      static_predicates_(static_predicates(predicates, actions)) {
    domains[name] = this;
  }

  /* Deletes a domain. */
  ~Domain() {
    domains.erase(name);
  }

  /* Returns the type with the given name, or NULL if it is undefined. */
  const Type* find_type(const string& name) const {
    TypeMap::const_iterator i = types_.find(name);
    return (i != types_.end()) ? (*i).second : NULL;
  }

  /* Returns the constant with the given name, or NULL if it is undefined. */
  const Name* find_constant(const string& name) const {
    NameMap::const_iterator i = constants_.find(name);
    return (i != constants_.end()) ? (*i).second : NULL;
  }

  /* Returns the predicate with the given name, or NULL if it is undefined. */
  const Predicate* find_predicate(const string& name) const {
    PredicateMap::const_iterator i = predicates_.find(name);
    return (i != predicates_.end()) ? (*i).second : NULL;
  }

  /* Returns the action with the given name, or NULL if it is undefined. */
  const Action* find_action(const string& name) const {
    ActionMap::const_iterator i = actions_.find(name);
    return (i != actions_.end()) ? (*i).second : NULL;
  }

  /* Tests if the given predicate is static. */
  bool static_predicate(const string& predicate) const {
    return (static_predicates_.find(predicate) != static_predicates_.end());
  }

  /* Fills the provided action list with actions that can achive the
     give goal. */
  void find_applicable_actions(ActionList& actions,
			       const Formula& goal) const {
    for (ActionMap::const_iterator i = actions_.begin();
	 i != actions_.end(); i++) {
      const Action& a = *(*i).second;
      if (a.applicable(goal)) {
	actions.push_back(&a);
      }
    }
  }

  /* Fills the provided name list with constants that are compatible
     with the given type. */
  void compatible_constants(NameList& constants,
			    const Type& t = SimpleType::OBJECT_TYPE) const {
    for (NameMap::const_iterator i = constants_.begin();
	 i != constants_.end(); i++) {
      const Name& name = *(*i).second;
      if (name.type.compatible(t)) {
	constants.push_back(&name);
      }
    }
  }

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Domain types. */
  const TypeMap& types_;
  /* Domain constants. */
  const NameMap& constants_;
  /* Domain predicates. */
  const PredicateMap& predicates_;
  /* Domain actions. */
  const ActionMap& actions_;
  /* Static predicates. */
  const hash_set<string> static_predicates_;

  /* Returns a set of static predicates. */
  static hash_set<string> static_predicates(const PredicateMap& predicates,
					    const ActionMap& actions) {
    hash_set<string> preds;
    for (PredicateMap::const_iterator i = predicates.begin();
	 i != predicates.end(); i++) {
      const string& p = (*i).first;
      bool is_static = true;
      for (ActionMap::const_iterator j = actions.begin();
	 j != actions.end() && is_static; j++) {
	if ((*j).second->effects.involves(p)) {
	  is_static = false;
	}
      }
      if (is_static) {
	preds.insert((*i).first);
      }
    }
    return preds;
  }
	  
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
