/* -*-C++-*- */
/*
 * Domain descriptions.
 *
 * $Id: domains.h,v 1.26 2001-12-23 15:50:04 lorens Exp $
 */
#ifndef DOMAINS_H
#define DOMAINS_H

#include <hash_set>
#include "support.h"
#include "types.h"


struct SubstitutionList;
struct TermList;
struct VariableList;
struct Name;
struct NameList;
struct NameMap;
struct Formula;
struct AtomList;
struct NegationList;
struct Atom;
struct Problem;

/*
 * Predicate declaration.
 */
struct Predicate : public Printable, public gc {
  /* Name of this predicate. */
  const string name;
  /* Predicate parameters. */
  const VariableList& parameters;

  /* Constructs a predicate with the given names and parameters. */
  Predicate(const string& name, const VariableList& params)
    : name(name), parameters(params) {}

  /* Returns the arity of this predicate. */
  size_t arity() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Table of predicate declarations.
 */
struct PredicateMap : public HashMap<string, const Predicate*> {
};

typedef PredicateMap::const_iterator PredicateMapIter;


struct EffectList;

/*
 * Effect definition.
 */
struct Effect : public Printable, public gc {
  /* List of universally quantified variables for this effect. */
  const VariableList& forall;
  /* Condition for this effect, or TRUE if unconditional effect. */
  const Formula& condition;
  /* Add list for this effect. */
  const AtomList& add_list;
  /* Delete list for this effect. */
  const NegationList& del_list;

  /* Constructs an unconditional effect. */
  Effect(const AtomList& add_list, const NegationList& del_list);

  /* Constructs a conditional effect. */
  Effect(const Formula& condition,
	 const AtomList& add_list, const NegationList& del_list);

  /* Constructs a universally quantified unconditional effect. */
  Effect(const VariableList& forall,
	 const AtomList& add_list, const NegationList& del_list);

  /* Constructs a universally quantified conditional effect. */
  Effect(const VariableList& forall, const Formula& condition,
	 const AtomList& add_list, const NegationList& del_list)
    : forall(forall), condition(condition),
      add_list(add_list), del_list(del_list) {}

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
struct EffectList : public Vector<const Effect*> {
  /* Constructs an empty effect list. */
  EffectList() {}

  /* Constructs an effect list with a single effect. */
  EffectList(const Effect* effect) {
    push_back(effect);
  }

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
};

typedef EffectList::const_iterator EffectListIter;


/*
 * Abstract action definition.
 */
struct Action : public Printable, public gc {
  /* Name of this action. */
  const string name;
  /* Action precondition. */
  const Formula& precondition;
  /* List of action effects. */
  const EffectList& effects;

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const = 0;

  /* Fills the provided sets with predicates achievable by this
     action. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

protected:
  /* Constructs an action. */
  Action(const string& name, const Formula& precondition,
	 const EffectList& effects)
    : name(name), precondition(precondition), effects(effects) {}
};


/*
 * List of action definitions.
 */
struct ActionList : public Vector<const Action*> {
};

typedef ActionList::const_iterator ActionListIter;


struct GroundActionList;
struct Domain;

/*
 * Action schema definition.
 */
struct ActionSchema : public Action {
  /* Action schema parameters. */
  const VariableList& parameters;

  ActionSchema(const string& name, const VariableList& parameters,
	       const Formula& precondition, const EffectList& effects)
    : Action(name, precondition, effects), parameters(parameters) {}

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const;

  /* Fills the provided action list with all instantiations of this
     action schema. */
  void instantiations(GroundActionList& actions, const Problem& problem) const;

  /* Returns this action schema with all static preconditions assumed
     true. */
  const ActionSchema& strip_static(const Domain& domain) const;

  /* Returns this action schema with all equality/inequality
     preconditions assumed true. */
  const ActionSchema& strip_equality() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Table of action schema definitions.
 */
struct ActionSchemaMap : public HashMap<string, const ActionSchema*> {
};

typedef ActionSchemaMap::const_iterator ActionSchemaMapIter;


/*
 * Ground action.
 */
struct GroundAction : public Action {
  /* Action arguments. */
  const NameList& arguments;

  /* Constructs a ground action, assuming arguments are names. */
  GroundAction(const string& name, const NameList& arguments,
	       const Formula& precondition, const EffectList& effects)
    : Action(name, precondition, effects), arguments(arguments),
      formula(NULL) {}

  /* Returns a formula representing this action. */
  virtual const Atom& action_formula() const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Atomic representation of this ground action. */
  mutable const Atom* formula;
};


/*
 * List of ground actions.
 */
struct GroundActionList : public Vector<const GroundAction*> {
};

typedef GroundActionList::const_iterator GroundActionListIter;


struct Requirements;

/*
 * Domain definition.
 */
struct Domain : public Printable, public gc {
  /* Table of domain definitions. */
  struct DomainMap : public HashMap<string, const Domain*> {
  };

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
  /* Domain action schemas. */
  const ActionSchemaMap& actions;

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
	 const PredicateMap& predicates, const ActionSchemaMap& actions)
    : name(name), requirements(requirements), types(types),
      constants(constants), predicates(predicates), actions(actions),
      static_predicates_(static_predicates(predicates, actions)) {
    domains[name] = this;
  }

  /* Deletes a domain. */
  virtual ~Domain() {
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

  /* Returns the action schema with the given name, or NULL if it is
     undefined. */
  const ActionSchema* find_action(const string& name) const;

  /* Fills the provided name list with constants that are compatible
     with the given type. */
  void compatible_constants(NameList& constants,
			    const Type& t = SimpleType::OBJECT) const;

  /* Tests if the given predicate is static. */
  bool static_predicate(const string& predicate) const {
    return (static_predicates_.find(predicate) != static_predicates_.end());
  }

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Static predicates. */
  const hash_set<string> static_predicates_;

  /* Returns a set of static predicates. */
  static hash_set<string> static_predicates(const PredicateMap& predicates,
					    const ActionSchemaMap& actions);
};

#endif /* DOMAINS_H */
