/*
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
 * $Id: domains.cc,v 2.1 2002-02-07 19:05:42 lorens Exp $
 */
#include "domains.h"
#include "problems.h"
#include "formulas.h"
#include "types.h"


/* ====================================================================== */
/* Predicate */

/* Constructs a predicate with the given names and parameters. */
Predicate::Predicate(const string& name, const VariableList& params)
  : name(name), parameters(params) {}


/* Returns the arity of this predicate. */
size_t Predicate::arity() const {
  return parameters.size();
}


/* Prints this object on the given stream. */
void Predicate::print(ostream& os) const {
  os << '(' << name;
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    os << ' ' << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ')';
}


/* ====================================================================== */
/* Effect */

/* Constructs an unconditional effect. */
Effect::Effect(const AtomList& add_list, const NegationList& del_list)
  : forall(VariableList::EMPTY), condition(Formula::TRUE),
    add_list(add_list), del_list(del_list) {
}


/* Constructs a conditional effect. */
Effect::Effect(const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list)
  : forall(VariableList::EMPTY), condition(condition),
    add_list(add_list), del_list(del_list) {
}


/* Constructs a universally quantified unconditional effect. */
Effect::Effect(const VariableList& forall,
	       const AtomList& add_list, const NegationList& del_list)
  : forall(forall), condition(Formula::TRUE),
    add_list(add_list), del_list(del_list) {
}


/* Constructs a universally quantified conditional effect. */
Effect::Effect(const VariableList& forall, const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list)
  : forall(forall), condition(condition),
    add_list(add_list), del_list(del_list) {}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(size_t id) const {
  return *(new Effect(forall.instantiation(id), condition.instantiation(id),
		      add_list.instantiation(id), del_list.instantiation(id)));
}


/* Fills the provided list with instantiations of this effect. */
void Effect::instantiations(EffectList& effects, const SubstitutionList& subst,
			    const Problem& problem) const {
  if (forall.empty()) {
    const Formula& new_condition = condition.instantiation(subst, problem);
    if (!new_condition.contradiction()) {
      effects.push_back(new Effect(new_condition,
				   add_list.substitution(subst),
				   del_list.substitution(subst)));
    }
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = **si;
      if (!member(forall.begin(), forall.end(), &s.var)) {
	args.push_back(&s);
      }
    }
    Vector<NameList*> arguments;
    Vector<NameListIter> next_arg;
    for (VarListIter vi = forall.begin(); vi != forall.end(); vi++) {
      arguments.push_back(new NameList());
      problem.compatible_objects(*arguments.back(), (*vi)->type);
      if (arguments.back()->empty()) {
	return;
      }
      next_arg.push_back(arguments.back()->begin());
    }
    size_t n = forall.size();
    for (size_t i = 0; i < n; ) {
      args.push_back(new Substitution(*forall[i], **next_arg[i]));
      const Formula& new_condition = condition.instantiation(args, problem);
      if (i + 1 == n || new_condition.contradiction()) {
	if (!new_condition.contradiction()) {
	  effects.push_back(new Effect(new_condition,
				       add_list.substitution(args),
				       del_list.substitution(args)));
	}
	for (int j = i; j >= 0; j--) {
	  args.pop_back();
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j]->end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j]->begin();
	    }
	  } else {
	    i = j;
	    break;
	  }
	}
      } else {
	i++;
      }
    }
  }
}


/* Returns this effect subject to the given substitutions. */
const Effect& Effect::substitution(const SubstitutionList& subst) const {
  if (forall.empty()) {
    return *(new Effect(forall, condition.substitution(subst),
			add_list.substitution(subst),
			del_list.substitution(subst)));
  } else {
    SubstitutionList eff_subst;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = **si;
      if (!member(forall.begin(), forall.end(), &s.var)) {
	eff_subst.push_back(&s);
      }
    }
    return *(new Effect(forall, condition.substitution(eff_subst),
			add_list.substitution(eff_subst),
			del_list.substitution(eff_subst)));
  }
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void Effect::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  for (AtomListIter gi = add_list.begin(); gi != add_list.end(); gi++) {
    preds.insert((*gi)->predicate());
  }
  for (NegationListIter gi = del_list.begin(); gi != del_list.end(); gi++) {
    neg_preds.insert((*gi)->predicate());
  }
}


/* Prints this object on the given stream. */
void Effect::print(ostream& os) const {
  os << '[';
  if (!condition.tautology()) {
    os << condition;
  }
  os << "->";
  if (add_list.size() + del_list.size() == 1) {
    if (!add_list.empty()) {
      os << *add_list.front();
    } else {
      os << *del_list.front();
    }
  } else {
    os << "(and";
    for (AtomListIter gi = add_list.begin(); gi != add_list.end(); gi++) {
      os << ' ' << **gi;
    }
    for (NegationListIter gi = del_list.begin(); gi != del_list.end(); gi++) {
      os << ' ' << **gi;
    }
    os << ")";
  }
  os << ']';
}


/* ====================================================================== */
/* EffectList */

/* An empty effect list. */
const EffectList& EffectList::EMPTY = *(new EffectList());


/* Constructs an empty effect list. */
EffectList::EffectList() {}


/* Constructs an effect list with a single effect. */
EffectList::EffectList(const Effect* effect)
  : Vector<const Effect*>(1, effect) {}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(size_t id) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    effects.push_back(&(*i)->instantiation(id));
  }
  return effects;
}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator ei = begin(); ei != end(); ei++) {
    (*ei)->instantiations(effects, subst, problem);
  }
  return effects;
}


/* Returns this effect list subject to the given substitutions. */
const EffectList&
EffectList::substitution(const SubstitutionList& subst) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    effects.push_back(&(*i)->substitution(subst));
  }
  return effects;
}


/* Fills the provided sets with predicates achievable by the effects
   in this list. */
void EffectList::achievable_predicates(hash_set<string>& preds,
				       hash_set<string>& neg_preds) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_predicates(preds, neg_preds);
  }
}


/* ====================================================================== */
/* Action */

/* Constructs an action. */
Action::Action(const string& name, const Formula& precondition,
	       const EffectList& effects)
  : name(name), precondition(precondition), effects(effects) {}


/* Fills the provided sets with predicates achievable by this
   action. */
void Action::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  effects.achievable_predicates(preds, neg_preds);
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema. */
ActionSchema::ActionSchema(const string& name, const VariableList& parameters,
			   const Formula& precondition,
			   const EffectList& effects)
  : Action(name, precondition, effects), parameters(parameters) {}


/* Returns a formula representing this action. */
const Atom& ActionSchema::action_formula() const {
  TermList& terms = *(new TermList());
  copy(parameters.begin(), parameters.end(), back_inserter(terms));
  return *(new Atom(name, terms));
}


/* Fills the provided action list with all instantiations of this
   action schema. */
void ActionSchema::instantiations(GroundActionList& actions,
				  const Problem& problem) const {
  size_t n = parameters.size();
  if (n == 0) {
    SubstitutionList args;
    actions.push_back(new GroundAction(name, *(new NameList()), precondition,
				       effects.instantiation(args, problem)));
    return;
  }
  Vector<NameList*> arguments;
  Vector<NameListIter> next_arg;
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*vi)->type);
    if (arguments.back()->empty()) {
      return;
    }
    next_arg.push_back(arguments.back()->begin());
  }
  Stack<const Formula*> preconds;
  preconds.push(&precondition);
  SubstitutionList args;
  for (size_t i = 0; i < n; ) {
    const Substitution* subst =
      new Substitution(*parameters[i], **next_arg[i]);
    args.push_back(subst);
    SubstitutionList pargs;
    pargs.push_back(subst);
    const Formula& new_precond = preconds.top()->instantiation(pargs, problem);
    preconds.push(&new_precond);
    if (i + 1 == n || new_precond.contradiction()) {
      if (!new_precond.contradiction()) {
	const EffectList& new_effects = effects.instantiation(args, problem);
	if (!new_effects.empty()) {
	  /* consistent instantiation */
	  NameList& names = *(new NameList());
	  for (SubstListIter si = args.begin(); si != args.end(); si++) {
	    const Name& name = dynamic_cast<const Name&>((*si)->term);
	    names.push_back(&name);
	  }
	  actions.push_back(new GroundAction(name, names, new_precond,
					     new_effects));
	}
      }
      for (int j = i; j >= 0; j--) {
	args.pop_back();
	preconds.pop();
	next_arg[j]++;
	if (next_arg[j] == arguments[j]->end()) {
	  if (j == 0) {
	    i = n;
	    break;
	  } else {
	    next_arg[j] = arguments[j]->begin();
	  }
	} else {
	  i = j;
	  break;
	}
      }
    } else {
      i++;
    }
  }
}


/* Returns this action schema with all static preconditions assumed
   true. */
const ActionSchema& ActionSchema::strip_static(const Domain& domain) const {
  return *(new ActionSchema(name, parameters,
			    precondition.strip_static(domain), effects));
}


/* Prints this object on the given stream. */
void ActionSchema::print(ostream& os) const {
  os << '(' << name << " (";
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    if (vi != parameters.begin()) {
      os << ' ';
    }
    os << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ") ";
  if (!precondition.tautology()) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    if (ei != effects.begin()) {
      os << ' ';
    }
    os << **ei;
  }
  os << ")" << ')';
}


/* ====================================================================== */
/* GroundAction */

/* Constructs a ground action, assuming arguments are names. */
GroundAction::GroundAction(const string& name, const NameList& arguments,
			   const Formula& precondition,
			   const EffectList& effects)
  : Action(name, precondition, effects), arguments(arguments),
    formula_(NULL) {}


/* Returns a formula representing this action. */
const Atom& GroundAction::action_formula() const {
  if (formula_ == NULL) {
    TermList& terms = *(new TermList());
    copy(arguments.begin(), arguments.end(), back_inserter(terms));
    formula_ = new Atom(name, terms);
  }
  return *formula_;
}


/* Prints this object on the given stream. */
void GroundAction::print(ostream& os) const {
  os << '(' << name << " (";
  for (NameListIter ni = arguments.begin(); ni != arguments.end(); ni++) {
    if (ni != arguments.begin()) {
      os << ' ';
    }
    os << **ni;
  }
  os << ") ";
  if (!precondition.tautology()) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    if (ei != effects.begin()) {
      os << ' ';
    }
    os << **ei;
  }
  os << ")" << ')';
}


/* ====================================================================== */
/* Domain */

/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Returns a const_iterator pointing to the first domain. */
Domain::DomainMapIter Domain::begin() {
  return domains.begin();
}


/* Returns a const_iterator pointing beyond the last domain. */
Domain::DomainMapIter Domain::end() {
  return domains.end();
}


/* Returns the domain with the given name, or NULL it is undefined. */
const Domain* Domain::find(const string& name) {
  DomainMapIter di = domains.find(name);
  return (di != domains.end()) ? (*di).second : NULL;
}


/* Removes all defined domains. */
void Domain::clear() {
  domains.clear();
}


/* Constructs a domain. */
Domain::Domain(const string& name, const Requirements& requirements,
	       const TypeMap& types, const NameMap& constants,
	       const PredicateMap& predicates, const ActionSchemaMap& actions)
  : name(name), requirements(requirements), types(types),
    constants(constants), predicates(predicates), actions(actions) {
  domains[name] = this;
  hash_set<string> static_preds;
  hash_set<string> achievable_preds;
  for (ActionSchemaMapIter ai = actions.begin(); ai != actions.end(); ai++) {
    (*ai).second->achievable_predicates(achievable_preds, achievable_preds);
  }
  for (PredicateMapIter pi = predicates.begin();
       pi != predicates.end(); pi++) {
    const string& p = (*pi).first;
    if (achievable_preds.find(p) == achievable_preds.end()) {
      static_predicates_.insert(p);
    }
  }
}


/* Deletes a domain. */
Domain::~Domain() {
  domains.erase(name);
}


/* Returns the type with the given name, or NULL if it is
   undefined. */
const Type* Domain::find_type(const string& name) const {
  TypeMapIter ti = types.find(name);
  return (ti != types.end()) ? (*ti).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
const Name* Domain::find_constant(const string& name) const {
  NameMapIter ni = constants.find(name);
  return (ni != constants.end()) ? (*ni).second : NULL;
}


/* Returns the predicate with the given name, or NULL if it is
   undefined. */
const Predicate* Domain::find_predicate(const string& name) const {
  PredicateMapIter pi = predicates.find(name);
  return (pi != predicates.end()) ? (*pi).second : NULL;
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const string& name) const {
  ActionSchemaMapIter ai = actions.find(name);
  return (ai != actions.end()) ? (*ai).second : NULL;
}


/* Fills the provided name list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(NameList& constants, const Type& t) const {
  for (NameMapIter ni = this->constants.begin();
       ni != this->constants.end(); ni++) {
    const Name& name = *(*ni).second;
    if (name.type.subtype(t)) {
      constants.push_back(&name);
    }
  }
}


/* Tests if the given predicate is static. */
bool Domain::static_predicate(const string& predicate) const {
  return (static_predicates_.find(predicate) != static_predicates_.end());
}


/* Prints this object on the given stream. */
void Domain::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "types:";
  for (TypeMapIter ti = types.begin(); ti != types.end(); ti++) {
    if (!(*ti).second->object()) {
      os << ' ' << *(*ti).second;
      if (!(*ti).second->supertype.object()) {
	os << " - " << (*ti).second->supertype;
      }
    }
  }
  os << endl << "constants:";
  for (NameMapIter ni = constants.begin(); ni != constants.end(); ni++) {
    os << ' ' << *(*ni).second;
    if (!(*ni).second->type.object()) {
      os << " - " << (*ni).second->type;
    }
  }
  os << endl << "predicates:";
  for (PredicateMapIter pi = predicates.begin();
       pi != predicates.end(); pi++) {
    os << endl << "  " << *(*pi).second;
  }
  os << endl << "actions:";
  for (ActionSchemaMapIter ai = actions.begin(); ai != actions.end(); ai++) {
    os << endl << "  " << *(*ai).second;
  }
}
