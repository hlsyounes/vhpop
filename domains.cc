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
 * $Id: domains.cc,v 4.1 2002-07-22 22:42:58 lorens Exp $
 */
#include <stack>
#include "bindings.h"
#include "domains.h"
#include "problems.h"


/* ====================================================================== */
/* Predicate */

/* Constructs a predicate with the given name. */
Predicate::Predicate(const string& name)
  : name_(name) {}


/* Deletes this predicate. */
Predicate::~Predicate() {
  for (TypeListIter ti = parameters_.begin(); ti != parameters_.end(); ti++) {
    const UnionType* ut = dynamic_cast<const UnionType*>(*ti);
    if (ut != NULL) {
      delete ut;
    }
  }
}


/* Returns the arity of this predicate. */
size_t Predicate::arity() const {
  return parameters_.size();
}


/* Predicate parameters. */
const Type& Predicate::type(size_t i) const {
  return *parameters_[i];
}


/* Adds a parameter to this predicate. */
void Predicate::add_parameter(const Type& type) {
  parameters_.push_back(&type);
}


/* Equality operator for predicates. */
bool operator==(const Predicate& p1, const Predicate& p2) {
  return &p1 == &p2;
}


/* Output operator for predicates. */
ostream& operator<<(ostream& os, const Predicate& p) {
  os << '(' << p.name();
  size_t n = p.arity();
  for (size_t i = 0; i < n; i++) {
    os << " x? - " << p.type(i);
  }
  os << ')';
  return os;
}


/* ====================================================================== */
/* Effect */

/* Constructs an unconditional effect. */
Effect::Effect(const AtomList& add_list, const NegationList& del_list,
	       EffectTime when)
  : forall_(&VariableList::EMPTY), condition_(&Formula::TRUE),
    link_condition_(&Formula::TRUE),
    add_list_(&add_list), del_list_(&del_list), when_(when) {
}


/* Constructs a conditional effect. */
Effect::Effect(const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list,
	       EffectTime when)
  : forall_(&VariableList::EMPTY), condition_(&condition),
    link_condition_(&Formula::TRUE),
    add_list_(&add_list), del_list_(&del_list), when_(when) {
}


/* Constructs a universally quantified unconditional effect. */
Effect::Effect(const VariableList& forall,
	       const AtomList& add_list, const NegationList& del_list,
	       EffectTime when)
  : forall_(&forall), condition_(&Formula::TRUE),
    link_condition_(&Formula::TRUE),
    add_list_(&add_list), del_list_(&del_list), when_(when) {
}


/* Constructs a universally quantified conditional effect. */
Effect::Effect(const VariableList& forall, const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list,
	       EffectTime when)
  : forall_(&forall), condition_(&condition), link_condition_(&Formula::TRUE),
    add_list_(&add_list), del_list_(&del_list), when_(when) {}


/* Constructs a universally quantified conditional effect with a
   link condition. */
Effect::Effect(const VariableList& forall, const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list,
	       EffectTime when, const Formula& link_condition)
  : forall_(&forall), condition_(&condition), link_condition_(&link_condition),
    add_list_(&add_list), del_list_(&del_list), when_(when) {}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(size_t id) const {
  return *(new Effect(forall().instantiation(id),
		      condition().instantiation(id),
		      add_list().instantiation(id),
		      del_list().instantiation(id),
		      when(), link_condition().instantiation(id)));
}


/* Fills the provided list with instantiations of this effect. */
void Effect::instantiations(EffectList& effects, const SubstitutionList& subst,
			    const Problem& problem) const {
  if (forall().empty()) {
    const Formula& new_condition = condition().instantiation(subst, problem);
    if (!new_condition.contradiction()) {
      effects.push_back(new Effect(forall(), new_condition,
				   add_list().substitution(subst),
				   del_list().substitution(subst),
				   when(),
				   link_condition().instantiation(subst,
								  problem)));
    }
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = *si;
      if (!member(forall().begin(), forall().end(), &s.var())) {
	args.push_back(s);
      }
    }
    vector<NameList*> arguments;
    vector<NameListIter> next_arg;
    for (VarListIter vi = forall().begin(); vi != forall().end(); vi++) {
      arguments.push_back(new NameList());
      problem.compatible_objects(*arguments.back(), (*vi)->type());
      if (arguments.back()->empty()) {
	return;
      }
      next_arg.push_back(arguments.back()->begin());
    }
    size_t n = forall().size();
    for (size_t i = 0; i < n; ) {
      args.push_back(Substitution(*forall()[i], **next_arg[i]));
      const Formula& new_condition = condition().instantiation(args, problem);
      if (i + 1 == n || new_condition.contradiction()) {
	if (!new_condition.contradiction()) {
	  effects.push_back(new Effect(VariableList::EMPTY, new_condition,
				       add_list().substitution(args),
				       del_list().substitution(args),
				       when(),
				       link_condition().instantiation(args,
								      problem)));
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
  if (forall().empty()) {
    return *(new Effect(forall(), condition().substitution(subst),
			add_list().substitution(subst),
			del_list().substitution(subst),
			when(), link_condition().substitution(subst)));
  } else {
    SubstitutionList eff_subst;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = *si;
      if (!member(forall().begin(), forall().end(), &s.var())) {
	eff_subst.push_back(s);
      }
    }
    return *(new Effect(forall(), condition().substitution(eff_subst),
			add_list().substitution(eff_subst),
			del_list().substitution(eff_subst),
			when(), link_condition().substitution(eff_subst)));
  }
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void
Effect::achievable_predicates(PredicateSet& preds,
			      PredicateSet& neg_preds) const {
  for (AtomListIter gi = add_list().begin(); gi != add_list().end(); gi++) {
    preds.insert(&(*gi)->predicate());
  }
  for (NegationListIter gi = del_list().begin();
       gi != del_list().end(); gi++) {
    neg_preds.insert(&(*gi)->predicate());
  }
}


/* Returns a copy of this effect with a new link condition. */
const Effect& Effect::new_link_condition(const Formula& cond) const {
  return *(new Effect(forall(), condition(), add_list(), del_list(), when(),
		      cond));
}


/* Output operator for effects. */
ostream& operator<<(ostream& os, const Effect& e) {
  os << '(';
  switch (e.when()) {
  case Effect::AT_START:
    os << "at start ";
    break;
  case Effect::AT_END:
    os << "at end ";
    break;
  }
  os << '[';
  if (!e.condition().tautology()) {
    os << e.condition();
  }
  os << ',';
  if (!e.link_condition().tautology()) {
    os << e.link_condition();
  }
  os << "->";
  if (e.add_list().size() + e.del_list().size() == 1) {
    if (!e.add_list().empty()) {
      os << *e.add_list().front();
    } else {
      os << *e.del_list().front();
    }
  } else {
    os << "(and";
    copy(e.add_list().begin(), e.add_list().end(),
	 pre_ostream_iterator<Atom>(os));
    copy(e.del_list().begin(), e.del_list().end(),
	 pre_ostream_iterator<Negation>(os));
    os << ")";
  }
  os << ']' << ')';
  return os;
}


/* ====================================================================== */
/* EffectList */

/* An empty effect list. */
const EffectList EffectList::EMPTY = EffectList();


/* Constructs an empty effect list. */
EffectList::EffectList() {}


/* Constructs an effect list with a single effect. */
EffectList::EffectList(const Effect* effect)
  : vector<const Effect*>(1, effect) {}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(size_t id) const {
  if (empty()) {
    return EMPTY;
  } else {
    EffectList& effects = *(new EffectList());
    for (const_iterator i = begin(); i != end(); i++) {
      effects.push_back(&(*i)->instantiation(id));
    }
    return effects;
  }
}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  if (empty()) {
    return EMPTY;
  } else {
    EffectList& effects = *(new EffectList());
    for (const_iterator ei = begin(); ei != end(); ei++) {
      (*ei)->instantiations(effects, subst, problem);
    }
    return effects;
  }
}


/* Returns this effect list subject to the given substitutions. */
const EffectList&
EffectList::substitution(const SubstitutionList& subst) const {
  if (empty()) {
    return EMPTY;
  } else {
    EffectList& effects = *(new EffectList());
    for (const_iterator i = begin(); i != end(); i++) {
      effects.push_back(&(*i)->substitution(subst));
    }
    return effects;
  }
}


/* Fills the provided sets with predicates achievable by the effects
   in this list. */
void EffectList::achievable_predicates(PredicateSet& preds,
				       PredicateSet& neg_preds) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_predicates(preds, neg_preds);
  }
}


/* "Strengthens" this effect list. */
const EffectList& EffectList::strengthen(const Formula& condition) const {
  /*
   * First make sure there is only one add or del per effect.
   */
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    const Effect& ei = **i;
    if (ei.add_list().size() + ei.del_list().size() == 1) {
      effects.push_back(&ei);
    } else if (ei.add_list().size() + ei.del_list().size() > 1) {
      for (AtomListIter ai = ei.add_list().begin();
	   ai != ei.add_list().end(); ai++) {
	AtomList& add_list = *(new AtomList());
	add_list.push_back(*ai);
	effects.push_back(new Effect(ei.forall(), ei.condition(), add_list,
				     NegationList::EMPTY, ei.when()));
      }
      for (NegationListIter ni = ei.del_list().begin();
	   ni != ei.del_list().end(); ni++) {
	NegationList& del_list = *(new NegationList());
	del_list.push_back(*ni);
	effects.push_back(new Effect(ei.forall(), ei.condition(),
				     AtomList::EMPTY, del_list, ei.when()));
      }
    }
  }

  /*
   * Separate negative effects from positive effects occuring at the
   * same time.
   */
  for (size_t i = 0; i < effects.size(); i++) {
    const Effect& ei = *effects[i];
    if (!ei.del_list().empty()) {
      const Negation& neg = *ei.del_list().back();
      const Formula* cond = &Formula::TRUE;
      for (const_iterator j = effects.begin();
	   j != effects.end() && !cond->contradiction(); j++) {
	const Effect& ej = **j;
	if (ei.when() == ej.when()
	    && ej.condition().tautology() && !ej.add_list().empty()) {
	  const Atom& atom = *ej.add_list().back();
	  SubstitutionList mgu;
	  if (Bindings::unifiable(mgu, neg.atom(), atom)) {
	    const Formula* sep = &Formula::FALSE;
	    for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	      const Substitution& subst = *si;
	      const Variable* var = &subst.var();
	      if (!member(ej.forall().begin(), ej.forall().end(), var)) {
		var = dynamic_cast<const Variable*>(&subst.term());
		if (var == NULL
		    || !member(ej.forall().begin(), ej.forall().end(), var)) {
		  if (subst.var() != subst.term()) {
		    sep = &(*sep
			    || *(new Inequality(subst.var(), subst.term())));
		  }
		}
	      }
	    }
	    cond = &(*cond && *sep);
	  }
	}
      }
      if (!cond->tautology()) {
	effects[i] = &ei.new_link_condition(*cond);
      }
    }
  }

  /*
   * Separate effects from conditions asserted at the same time.
   */
  for (size_t i = 0; i < effects.size(); i++) {
    const Effect& ei = *effects[i];
    const Literal* literal;
    if (!ei.add_list().empty()) {
      literal = ei.add_list().back();
    } else {
      literal = ei.del_list().back();
    }
    const Formula& cond = condition.separate(*literal);
    if (!cond.tautology()) {
      effects[i] = &ei.new_link_condition(cond);
    }
  }

  return effects;
}


/* ====================================================================== */
/* Action */

/* Constructs an action. */
Action::Action(const string& name, const Formula& precondition,
	       const EffectList& effects)
  : name_(name), precondition_(&precondition), effects_(&effects),
    durative_(false), min_duration_(0.0f), max_duration_(0.0f) {}


/* Constructs an action. */
Action::Action(const string& name, const Formula& precondition,
	       const EffectList& effects,
	       float min_duration, float max_duration)
  : name_(name), precondition_(&precondition), effects_(&effects),
    durative_(true), min_duration_(min_duration),
    max_duration_(max_duration) {}


/* Fills the provided sets with predicates achievable by this
   action. */
void Action::achievable_predicates(PredicateSet& preds,
				   PredicateSet& neg_preds) const {
  if (min_duration() <= max_duration()) {
    effects().achievable_predicates(preds, neg_preds);
  }
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema. */
ActionSchema::ActionSchema(const string& name, const VariableList& parameters,
			   const Formula& precondition,
			   const EffectList& effects)
  : Action(name, precondition, effects), parameters_(&parameters) {}

/* Constructs an action schema for a durative action. */
ActionSchema::ActionSchema(const string& name, const VariableList& parameters,
			   const Formula& precondition,
			   const EffectList& effects,
			   float min_duration, float max_duration)
  : Action(name, precondition, effects, min_duration, max_duration),
    parameters_(&parameters) {}


/* Returns a formula representing this action. */
const Atom& ActionSchema::action_formula() const {
  TermList& terms = *(new TermList());
  copy(parameters().begin(), parameters().end(), back_inserter(terms));
  return *(new Atom(*(new Predicate(name())), terms, Formula::AT_START));
}


/* Fills the provided action list with all instantiations of this
   action schema. */
void ActionSchema::instantiations(GroundActionList& actions,
				  const Problem& problem) const {
  if (min_duration() > max_duration()) {
    return;
  }

  size_t n = parameters().size();
  if (n == 0) {
    SubstitutionList args;
    if (durative()) {
      actions.push_back(new GroundAction(name(), *(new NameList()),
					 precondition(),
					 effects().instantiation(args,
								 problem),
					 min_duration(), max_duration()));
    } else {
      actions.push_back(new GroundAction(name(), *(new NameList()),
					 precondition(),
					 effects().instantiation(args,
								 problem)));
    }
    return;
  }
  vector<NameList*> arguments;
  vector<NameListIter> next_arg;
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*vi)->type());
    if (arguments.back()->empty()) {
      return;
    }
    next_arg.push_back(arguments.back()->begin());
  }
  stack<const Formula*> preconds;
  preconds.push(&precondition());
  SubstitutionList args;
  for (size_t i = 0; i < n; ) {
    args.push_back(Substitution(*parameters()[i], **next_arg[i]));
    SubstitutionList pargs;
    pargs.push_back(Substitution(*parameters()[i], **next_arg[i]));
    const Formula& new_precond = preconds.top()->instantiation(pargs, problem);
    preconds.push(&new_precond);
    if (i + 1 == n || new_precond.contradiction()) {
      if (!new_precond.contradiction()) {
	const EffectList& new_effects = effects().instantiation(args, problem);
	if (!new_effects.empty()) {
	  /* consistent instantiation */
	  NameList& names = *(new NameList());
	  for (SubstListIter si = args.begin(); si != args.end(); si++) {
	    const Name& name = dynamic_cast<const Name&>((*si).term());
	    names.push_back(&name);
	  }
	  if (durative()) {
	    actions.push_back(new GroundAction(name(), names, new_precond,
					       new_effects,
					       min_duration(),
					       max_duration()));
	  } else {
	    actions.push_back(new GroundAction(name(), names, new_precond,
					       new_effects));
	  }
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
  if (durative()) {
    return *(new ActionSchema(name(), parameters(),
			      precondition().strip_static(domain), effects(),
			      min_duration(), max_duration()));
  } else {
    return *(new ActionSchema(name(), parameters(),
			      precondition().strip_static(domain), effects()));
  }
}


/* Prints this object on the given stream. */
void ActionSchema::print(ostream& os) const {
  os << '(' << name() << " (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << **vi << " - " << (*vi)->type();
  }
  os << ") ";
  if (!precondition().tautology()) {
    os << precondition();
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    if (ei != effects().begin()) {
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
  : Action(name, precondition, effects), arguments_(&arguments),
    formula_(NULL) {}


/* Constructs a ground durative action, assuming arguments are names. */
GroundAction::GroundAction(const string& name, const NameList& arguments,
			   const Formula& precondition,
			   const EffectList& effects,
			   float min_duration, float max_duration)
  : Action(name, precondition, effects, min_duration, max_duration),
    arguments_(&arguments), formula_(NULL) {}


/* Returns a formula representing this action. */
const Atom& GroundAction::action_formula() const {
  if (formula_ == NULL) {
    TermList& terms = *(new TermList());
    copy(arguments().begin(), arguments().end(), back_inserter(terms));
    formula_ = new Atom(*(new Predicate(name())), terms, Formula::AT_START);
  }
  return *formula_;
}


/* Prints this object on the given stream. */
void GroundAction::print(ostream& os) const {
  os << '(' << name() << " (";
  for (NameListIter ni = arguments().begin(); ni != arguments().end(); ni++) {
    if (ni != arguments().begin()) {
      os << ' ';
    }
    os << **ni;
  }
  os << ") ";
  if (!precondition().tautology()) {
    os << precondition();
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    if (ei != effects().begin()) {
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
  DomainMapIter di = begin();
  while (di != end()) {
    delete (*di).second;
    di = begin();
  }
  domains.clear();
}


/* Constructs an empty domain with the given name. */
Domain::Domain(const string& name)
  : name_(name) {
  const Domain* d = find(name);
  if (d != NULL) {
    delete d;
  }
  domains[name] = this;
}


/* Deletes a domain. */
Domain::~Domain() {
  domains.erase(name());
  for (ActionSchemaMapIter ai = actions_.begin(); ai != actions_.end(); ai++) {
    delete (*ai).second;
  }
  for (PredicateMapIter pi = predicates_.begin();
       pi != predicates_.end(); pi++) {
    delete (*pi).second;
  }
  for (NameMapIter ni = constants_.begin(); ni != constants_.end(); ni++) {
    delete (*ni).second;
  }
  /* Delete supertypes that are union types first.  Would like to do
     this in the destructor of the simple type, but that causes core
     dump if supertype is a simple type that has been deleted. */
  for (TypeMapIter ti = types_.begin(); ti != types_.end(); ti++) {
    const UnionType* ut =
      dynamic_cast<const UnionType*>(&(*ti).second->supertype());
    if (ut != NULL) {
      delete ut;
    }
  }
  for (TypeMapIter ti = types_.begin(); ti != types_.end(); ti++) {
    if (!(*ti).second->object()) {
      delete (*ti).second;
    }
  }
}


/* Domain actions. */
const ActionSchemaMap& Domain::actions() const {
  return actions_;
}


/* Adds a type to this domain. */
void Domain::add_type(const SimpleType& type) {
  types_.insert(make_pair(type.name(), &type));
}


/* Adds a constant to this domain. */
void Domain::add_constant(const Name& constant) {
  constants_[constant.name()] = &constant;
}


/* Adds a predicate to this domain. */
void Domain::add_predicate(const Predicate& predicate) {
  predicates_.insert(make_pair(predicate.name(), &predicate));
  static_predicates_.insert(&predicate);
}


/* Adds an action to this domain. */
void Domain::add_action(const ActionSchema& action) {
  actions_.insert(make_pair(action.name(), &action));
  PredicateSet achievable_preds;
  action.achievable_predicates(achievable_preds, achievable_preds);
  for (PredicateSetIter pi = achievable_preds.begin();
       pi != achievable_preds.end(); pi++) {
    static_predicates_.erase(*pi);
  }
}


/* Returns the type with the given name, or NULL if it is
   undefined. */
const SimpleType* Domain::find_type(const string& name) const {
  TypeMapIter ti = types_.find(name);
  return (ti != types_.end()) ? (*ti).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
const Name* Domain::find_constant(const string& name) const {
  NameMapIter ni = constants_.find(name);
  return (ni != constants_.end()) ? (*ni).second : NULL;
}


/* Returns the predicate with the given name, or NULL if it is
   undefined. */
const Predicate* Domain::find_predicate(const string& name) const {
  PredicateMapIter pi = predicates_.find(name);
  return (pi != predicates_.end()) ? (*pi).second : NULL;
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const string& name) const {
  ActionSchemaMapIter ai = actions_.find(name);
  return (ai != actions_.end()) ? (*ai).second : NULL;
}


/* Fills the provided name list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(NameList& constants, const Type& t) const {
  for (NameMapIter ni = this->constants_.begin();
       ni != this->constants_.end(); ni++) {
    const Name& name = *(*ni).second;
    if (name.type().subtype(t)) {
      constants.push_back(&name);
    }
  }
}


/* Tests if the given predicate is static. */
bool Domain::static_predicate(const Predicate& predicate) const {
  return (static_predicates_.find(&predicate) != static_predicates_.end());
}


/* Output operator for domains. */
ostream& operator<<(ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << endl << "types:";
  for (TypeMapIter ti = d.types_.begin(); ti != d.types_.end(); ti++) {
    if (!(*ti).second->object()) {
      os << ' ' << *(*ti).second << " - " << (*ti).second->supertype();
    }
  }
  os << endl << "constants:";
  for (NameMapIter ni = d.constants_.begin(); ni != d.constants_.end(); ni++) {
    os << ' ' << *(*ni).second << " - " << (*ni).second->type();
  }
  os << endl << "predicates:";
  for (PredicateMapIter pi = d.predicates_.begin();
       pi != d.predicates_.end(); pi++) {
    os << endl << "  " << *(*pi).second;
  }
  os << endl << "actions:";
  for (ActionSchemaMapIter ai = d.actions_.begin();
       ai != d.actions_.end(); ai++) {
    os << endl << "  " << *(*ai).second;
  }
  return os;
}
