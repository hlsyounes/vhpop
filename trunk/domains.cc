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
 * $Id: domains.cc,v 4.7 2002-12-16 17:06:45 lorens Exp $
 */
#include "domains.h"
#include "bindings.h"
#include "problems.h"
#include "mathport.h"
#include <stack>
#include <iostream>


/* ====================================================================== */
/* Predicate */

/* Constructs a predicate with the given name. */
Predicate::Predicate(const std::string& name)
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


/* Output operator for predicates. */
std::ostream& operator<<(std::ostream& os, const Predicate& p) {
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

/* Constructs an empty effect. */
Effect::Effect(EffectTime when)
  : condition_(&Formula::TRUE), link_condition_(&Formula::TRUE), when_(when) {
  Formula::register_use(condition_);
  Formula::register_use(link_condition_);
}


/* Deletes this effect. */
Effect::~Effect() {
  for (VarListIter vi = forall().begin(); vi != forall().end(); vi++) {
    Variable::unregister_use(*vi);
  }
  Formula::unregister_use(condition_);
  Formula::unregister_use(link_condition_);
  for (AtomListIter ai = add_list().begin(); ai != add_list().end(); ai++) {
    Formula::unregister_use(*ai);
  }
  for (NegationListIter ni = del_list().begin();
       ni != del_list().end(); ni++) {
    Formula::unregister_use(*ni);
  }
}


/* Adds a universally quantified variable to this effect. */
void Effect::add_forall(const Variable& parameter) {
  forall_.push_back(&parameter);
  Variable::register_use(&parameter);
}


/* Sets the condition of this effect. */
void Effect::set_condition(const Formula& condition) {
  if (condition_ != &condition) {
    Formula::unregister_use(condition_);
    condition_ = &condition;
    Formula::register_use(condition_);
  }
}


/* Sets the link condition of this effect. */
void Effect::set_link_condition(const Formula& link_condition) const {
  if (link_condition_ != &link_condition) {
    Formula::unregister_use(link_condition_);
    link_condition_ = &link_condition;
    Formula::register_use(link_condition_);
  }
}


/* Adds an atom to the add list of this effect. */
void Effect::add_positive(const Atom& atom) {
  add_list_.push_back(&atom);
  Formula::register_use(&atom);
}


/* Adds a negated atom to the delete list of this effect. */
void Effect::add_negative(const Negation& negation) {
  del_list_.push_back(&negation);
  Formula::register_use(&negation);
}


/* Fills the provided list with instantiations of this effect. */
void Effect::instantiations(EffectList& effects, const SubstitutionList& subst,
			    const Problem& problem) const {
  size_t n = forall().size();
  if (n == 0) {
    const Formula& inst_cond = condition().instantiation(subst, problem);
    if (!inst_cond.contradiction()) {
      const Effect* inst_effect = instantiation(subst, problem, inst_cond);
      if (inst_effect != NULL) {
	effects.push_back(inst_effect);
      } else {
	Formula::register_use(&inst_cond);
	Formula::unregister_use(&inst_cond);
      }
    }
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = *si;
      if (find(forall().begin(), forall().end(), &s.var()) == forall().end()) {
	args.push_back(s);
      }
    }
    std::vector<NameList> arguments(n, NameList());
    std::vector<NameListIter> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i], forall()[i]->type());
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const Formula*> conds;
    conds.push(&condition().instantiation(args, problem));
    Formula::register_use(conds.top());
    for (size_t i = 0; i < n; ) {
      args.push_back(Substitution(*forall()[i], **next_arg[i]));
      SubstitutionList pargs;
      pargs.push_back(Substitution(*forall()[i], **next_arg[i]));
      const Formula& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Formula::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
	if (!inst_cond.contradiction()) {
	  const Effect* inst_effect = instantiation(args, problem, inst_cond);
	  if (inst_effect != NULL) {
	    effects.push_back(inst_effect);
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Formula::unregister_use(conds.top());
	  conds.pop();
	  args.pop_back();
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
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
    while (!conds.empty()) {
      Formula::unregister_use(conds.top());
      conds.pop();
    }
  }
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void Effect::achievable_predicates(PredicateSet& preds,
				   PredicateSet& neg_preds) const {
  for (AtomListIter gi = add_list().begin(); gi != add_list().end(); gi++) {
    preds.insert(&(*gi)->predicate());
  }
  for (NegationListIter gi = del_list().begin();
       gi != del_list().end(); gi++) {
    neg_preds.insert(&(*gi)->predicate());
  }
}



/* Returns an instantiation of this effect. */
const Effect* Effect::instantiation(const SubstitutionList& args,
				    const Problem& problem,
				    const Formula& condition) const {
  const Formula& inst_link_cond =
    link_condition().instantiation(args, problem);
  if (!inst_link_cond.contradiction()
      && !(add_list().empty() && del_list().empty())) {
    Effect& inst_eff = *(new Effect(when()));
    inst_eff.set_condition(condition);
    inst_eff.set_link_condition(inst_link_cond);
    for (AtomListIter ai = add_list().begin(); ai != add_list().end(); ai++) {
      inst_eff.add_positive((*ai)->substitution(args, 0));
    }
    for (NegationListIter ni = del_list().begin();
	 ni != del_list().end(); ni++) {
      inst_eff.add_negative((*ni)->substitution(args, 0));
    }
    return &inst_eff;
  } else {
    Formula::register_use(&inst_link_cond);
    Formula::unregister_use(&inst_link_cond);
    return NULL;
  }
}


/* Output operator for effects. */
std::ostream& operator<<(std::ostream& os, const Effect& e) {
  os << '(';
  for (VarListIter vi = e.forall().begin(); vi != e.forall().end(); vi++) {
    os << **vi << ' ';
  }
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
    for (AtomListIter ai = e.add_list().begin();
	 ai != e.add_list().end(); ai++) {
      os << ' ' << **ai;
    }
    for (NegationListIter ni = e.del_list().begin();
	 ni != e.del_list().end(); ni++) {
      os << ' ' << **ni;
    }
    os << ")";
  }
  os << ']' << ')';
  return os;
}


/* ====================================================================== */
/* Action */

/* Constructs an action with the given name. */
Action::Action(const std::string& name, bool durative)
  : name_(name), precondition_(&Formula::TRUE), durative_(durative),
    min_duration_(0.0f), max_duration_(durative ? INFINITY : 0.0f) {
  Formula::register_use(precondition_);
}


/* Deletes this action. */
Action::~Action() {
  Formula::unregister_use(precondition_);
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    delete *ei;
  }
}


/* Sets the precondition for this action. */
void Action::set_precondition(const Formula& precondition) {
  if (precondition_ != &precondition) {
    Formula::unregister_use(precondition_);
    precondition_ = &precondition;
    Formula::register_use(precondition_);
  }
}


/* Adds an effect to this action. */
void Action::add_effect(const Effect& effect) {
  effects_.push_back(&effect);
}


/* Sets the minimum duration for this action. */
void Action::set_min_duration(double min_duration) {
  if (min_duration_ < min_duration) {
    min_duration_ = min_duration;
  }
}


/* Sets the maximum duration for this action. */
void Action::set_max_duration(double max_duration) {
  if (max_duration_ > max_duration) {
    max_duration_ = max_duration;
  }
}


/* Sets the duration for this action. */
void Action::set_duration(double duration) {
  set_min_duration(duration);
  set_max_duration(duration);
}


/* Fills the provided sets with predicates achievable by this
   action. */
void Action::achievable_predicates(PredicateSet& preds,
				   PredicateSet& neg_preds) const {
  if (min_duration() <= max_duration()) {
    for (EffectListIter ei = effects_.begin(); ei != effects_.end(); ei++) {
      (*ei)->achievable_predicates(preds, neg_preds);
    }
  }
}


/* "Strengthens" the effects of this action. */
void Action::strengthen_effects() {
  /*
   * First make sure there is only one add or del per effect.
   */
  EffectList old_effects(effects());
  effects_.clear();
  for (EffectListIter i = old_effects.begin(); i != old_effects.end(); i++) {
    const Effect& ei = **i;
    if (ei.add_list().size() + ei.del_list().size() == 1) {
      effects_.push_back(&ei);
    } else {
      for (AtomListIter ai = ei.add_list().begin();
	   ai != ei.add_list().end(); ai++) {
	Effect& one_eff = *(new Effect(ei.when()));
	for (VarListIter vi = ei.forall().begin();
	     vi != ei.forall().end(); vi++) {
	  one_eff.add_forall(**vi);
	}
	one_eff.set_condition(ei.condition());
	one_eff.add_positive(**ai);
	effects_.push_back(&one_eff);
      }
      for (NegationListIter ni = ei.del_list().begin();
	   ni != ei.del_list().end(); ni++) {
	Effect& one_eff = *(new Effect(ei.when()));
	for (VarListIter vi = ei.forall().begin();
	     vi != ei.forall().end(); vi++) {
	  one_eff.add_forall(**vi);
	}
	one_eff.set_condition(ei.condition());
	one_eff.add_negative(**ni);
	effects_.push_back(&one_eff);
      }
      delete &ei;
    }
  }

  /*
   * Separate negative effects from positive effects occuring at the
   * same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    if (!ei.del_list().empty()) {
      const Negation& neg = *ei.del_list().back();
      const Formula* cond = &Formula::TRUE;
      for (EffectListIter j = effects_.begin();
	   j != effects_.end() && !cond->contradiction(); j++) {
	const Effect& ej = **j;
	if (ei.when() == ej.when()
	    && ej.condition().tautology() && !ej.add_list().empty()) {
	  const Atom& atom = *ej.add_list().back();
	  SubstitutionList mgu;
	  if (Bindings::unifiable(mgu, neg.atom(), 0, atom, 0)) {
	    const Formula* sep = &Formula::FALSE;
	    for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	      const Substitution& subst = *si;
	      const Variable* var = &subst.var();
	      if (find(ej.forall().begin(), ej.forall().end(), var)
		  == ej.forall().end()) {
		var = dynamic_cast<const Variable*>(&subst.term());
		if (var == NULL
		    || (find(ej.forall().begin(), ej.forall().end(), var)
			== ej.forall().end())) {
		  if (subst.var() != subst.term()) {
		    sep =
		      &(*sep || *(new Inequality(subst.var(), subst.term())));
		  }
		}
	      }
	    }
	    cond = &(*cond && *sep);
	  }
	}
      }
      if (!cond->tautology()) {
	ei.set_link_condition(*cond);
      }
    }
  }

  /*
   * Separate effects from conditions asserted at the same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    const Literal* literal;
    if (!ei.add_list().empty()) {
      literal = ei.add_list().back();
    } else {
      literal = ei.del_list().back();
    }
    const Formula& cond = precondition().separate(*literal);
    if (!cond.tautology()) {
      ei.set_link_condition(ei.link_condition() && cond);
    }
  }
}


/* Output operator for actions. */
std::ostream& operator<<(std::ostream& os, const Action& a) {
  a.print(os);
  return os;
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Deletes this action schema. */
ActionSchema::~ActionSchema() {
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    Variable::unregister_use(*vi);
  }
}


/* Adds a parameter to this action schema. */
void ActionSchema::add_parameter(const Variable& var) {
  parameters_.push_back(&var);
  Variable::register_use(&var);
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
    const GroundAction* inst_action =
      instantiation(SubstitutionList(), problem, precondition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    SubstitutionList args;
    std::vector<NameList> arguments(n, NameList());
    std::vector<NameListIter> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i], parameters()[i]->type());
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const Formula*> preconds;
    preconds.push(&precondition());
    Formula::register_use(preconds.top());
    for (size_t i = 0; i < n; ) {
      args.push_back(Substitution(*parameters()[i], **next_arg[i]));
      SubstitutionList pargs;
      pargs.push_back(Substitution(*parameters()[i], **next_arg[i]));
      const Formula& inst_precond =
	preconds.top()->instantiation(pargs, problem);
      preconds.push(&inst_precond);
      Formula::register_use(preconds.top());
      if (i + 1 == n || inst_precond.contradiction()) {
	if (!inst_precond.contradiction()) {
	  const GroundAction* inst_action =
	    instantiation(args, problem, inst_precond);
	  if (inst_action != NULL) {
	    actions.push_back(inst_action);
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Formula::unregister_use(preconds.top());
	  preconds.pop();
	  args.pop_back();
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
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
    while (!preconds.empty()) {
      Formula::unregister_use(preconds.top());
      preconds.pop();
    }
  }
}


/* Returns an instantiation of this action schema. */
const GroundAction* ActionSchema::instantiation(const SubstitutionList& args,
						const Problem& problem,
						const Formula& precond) const {
  EffectList inst_effects;
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    (*ei)->instantiations(inst_effects, args, problem);
  }
  if (!inst_effects.empty()) {
    GroundAction& ga = *(new GroundAction(name(), durative()));
    for (SubstListIter si = args.begin(); si != args.end(); si++) {
      ga.add_argument(dynamic_cast<const Name&>((*si).term()));
    }
    ga.set_precondition(precond);
    for (EffectListIter ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(min_duration());
    ga.set_max_duration(max_duration());
    return &ga;
  } else {
    return NULL;
  }
}


/* Prints this action on the given stream with the given bindings. */
void ActionSchema::print(std::ostream& os, size_t step_id,
			 const Bindings* bindings) const {
  os << '(' << name();
  if (bindings != NULL) {
    for (VarListIter ti = parameters().begin();
	 ti != parameters().end(); ti++) {
      os << ' ';
      (*ti)->print(os, step_id, *bindings);
    }
  } else {
    for (VarListIter ti = parameters().begin();
	 ti != parameters().end(); ti++) {
      os << ' ' << **ti;
    }
  }
  os << ')';
}


/* Prints this object on the given stream. */
void ActionSchema::print(std::ostream& os) const {
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

/* Constructs a ground action with the given name. */
GroundAction::GroundAction(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Adds an argument to this ground action. */
void GroundAction::add_argument(const Name& arg) {
  arguments_.push_back(&arg);
}


/* Prints this action on the given stream with the given bindings. */
void GroundAction::print(std::ostream& os, size_t step_id,
			 const Bindings* bindings) const {
  os << '(' << name();
  for (NameListIter ni = arguments().begin(); ni != arguments().end(); ni++) {
    os << ' ' << **ni;
  }
  os << ')';
}


/* Prints this object on the given stream. */
void GroundAction::print(std::ostream& os) const {
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
const Domain* Domain::find(const std::string& name) {
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
Domain::Domain(const std::string& name)
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
void Domain::add_type(SimpleType& type) {
  types_.insert(make_pair(type.name(), &type));
}


/* Adds a constant to this domain. */
void Domain::add_constant(Name& constant) {
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
SimpleType* Domain::find_type(const std::string& name) {
  TypeMapIter ti = types_.find(name);
  return (ti != types_.end()) ? (*ti).second : NULL;
}


/* Returns the type with the given name, or NULL if it is
   undefined. */
const SimpleType* Domain::find_type(const std::string& name) const {
  TypeMapIter ti = types_.find(name);
  return (ti != types_.end()) ? (*ti).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
Name* Domain::find_constant(const std::string& name) {
  NameMapIter ni = constants_.find(name);
  return (ni != constants_.end()) ? (*ni).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
const Name* Domain::find_constant(const std::string& name) const {
  NameMapIter ni = constants_.find(name);
  return (ni != constants_.end()) ? (*ni).second : NULL;
}


/* Returns the predicate with the given name, or NULL if it is
   undefined. */
const Predicate* Domain::find_predicate(const std::string& name) const {
  PredicateMapIter pi = predicates_.find(name);
  return (pi != predicates_.end()) ? (*pi).second : NULL;
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const std::string& name) const {
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
  return (static_predicates_.find(&predicate) != static_predicates_.end()
	  || find_type(predicate.name()) != NULL);
}


/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << std::endl << "types:";
  for (TypeMapIter ti = d.types_.begin(); ti != d.types_.end(); ti++) {
    if (!(*ti).second->object()) {
      os << ' ' << *(*ti).second << " - " << (*ti).second->supertype();
    }
  }
  os << std::endl << "constants:";
  for (NameMapIter ni = d.constants_.begin(); ni != d.constants_.end(); ni++) {
    os << ' ' << *(*ni).second << " - " << (*ni).second->type();
  }
  os << std::endl << "predicates:";
  for (PredicateMapIter pi = d.predicates_.begin();
       pi != d.predicates_.end(); pi++) {
    os << std::endl << "  " << *(*pi).second;
  }
  os << std::endl << "actions:";
  for (ActionSchemaMapIter ai = d.actions_.begin();
       ai != d.actions_.end(); ai++) {
    os << std::endl << "  " << *(*ai).second;
  }
  return os;
}
