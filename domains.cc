/*
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
 * $Id: domains.cc,v 6.2 2003-07-21 02:01:55 lorens Exp $
 */
#include "domains.h"
#include "bindings.h"
#include "problems.h"
#include "mathport.h"
#include <stack>
#include <iostream>


/* ====================================================================== */
/* Effect */

/* Constructs an empty effect. */
Effect::Effect(EffectTime when)
  : condition_(&Condition::TRUE), link_condition_(&Condition::TRUE),
    when_(when) {
  Condition::register_use(condition_);
  Condition::register_use(link_condition_);
}


/* Deletes this effect. */
Effect::~Effect() {
  Condition::unregister_use(condition_);
  Condition::unregister_use(link_condition_);
  for (AtomListIter ai = add_list().begin(); ai != add_list().end(); ai++) {
    Formula::unregister_use(*ai);
  }
  for (NegationListIter ni = del_list().begin();
       ni != del_list().end(); ni++) {
    Formula::unregister_use(*ni);
  }
}


/* Adds a universally quantified variable to this effect. */
void Effect::add_forall(Variable parameter) {
  forall_.push_back(parameter);
}


/* Sets the condition of this effect. */
void Effect::set_condition(const Condition& condition) {
  if (condition_ != &condition) {
    Condition::unregister_use(condition_);
    condition_ = &condition;
    Condition::register_use(condition_);
  }
}


/* Sets the link condition of this effect. */
void Effect::set_link_condition(const Condition& link_condition) const {
  if (link_condition_ != &link_condition) {
    Condition::unregister_use(link_condition_);
    link_condition_ = &link_condition;
    Condition::register_use(link_condition_);
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
void Effect::instantiations(EffectList& effects, size_t& useful,
			    const SubstitutionMap& subst,
			    const Problem& problem) const {
  size_t n = forall().size();
  if (n == 0) {
    const Condition& inst_cond = condition().instantiation(subst, problem);
    if (!inst_cond.contradiction()) {
      const Effect* inst_effect = instantiation(subst, problem, inst_cond);
      if (inst_effect != NULL) {
	effects.push_back(inst_effect);
	if (!inst_effect->link_condition().contradiction()) {
	  useful++;
	}
      } else {
	Condition::register_use(&inst_cond);
	Condition::unregister_use(&inst_cond);
      }
    }
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.domain().terms().type(forall()[i]));
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const Condition*> conds;
    conds.push(&condition().instantiation(args, problem));
    Condition::register_use(conds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(forall()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(forall()[i], *next_arg[i]));
      const Condition& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Condition::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
	if (!inst_cond.contradiction()) {
	  const Effect* inst_effect = instantiation(args, problem, inst_cond);
	  if (inst_effect != NULL) {
	    effects.push_back(inst_effect);
	    if (!inst_effect->link_condition().contradiction()) {
	      useful++;
	    }
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Condition::unregister_use(conds.top());
	  conds.pop();
	  args.erase(forall()[j]);
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
      Condition::unregister_use(conds.top());
      conds.pop();
    }
  }
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void Effect::achievable_predicates(PredicateSet& preds,
				   PredicateSet& neg_preds) const {
  for (AtomListIter gi = add_list().begin(); gi != add_list().end(); gi++) {
    preds.insert((*gi)->predicate());
  }
  for (NegationListIter gi = del_list().begin();
       gi != del_list().end(); gi++) {
    neg_preds.insert((*gi)->predicate());
  }
}



/* Returns an instantiation of this effect. */
const Effect* Effect::instantiation(const SubstitutionMap& args,
				    const Problem& problem,
				    const Condition& condition) const {
  if (!(add_list().empty() && del_list().empty())) {
    Effect& inst_eff = *(new Effect(when()));
    inst_eff.set_condition(condition);
    inst_eff.set_link_condition(link_condition().instantiation(args, problem));
    for (AtomListIter ai = add_list().begin(); ai != add_list().end(); ai++) {
      inst_eff.add_positive((*ai)->substitution(args));
    }
    for (NegationListIter ni = del_list().begin();
	 ni != del_list().end(); ni++) {
      inst_eff.add_negative((*ni)->substitution(args));
    }
    return &inst_eff;
  } else {
    return NULL;
  }
}


/* Prints this effect on the given stream. */
void Effect::print(std::ostream& os, const PredicateTable& predicates,
		   const TermTable& terms) const {
  os << '(';
  for (VariableList::const_iterator vi = forall().begin();
       vi != forall().end(); vi++) {
    terms.print_term(os, *vi);
    os << ' ';
  }
  switch (when()) {
  case Effect::AT_START:
    os << "at start ";
    break;
  case Effect::AT_END:
    os << "at end ";
    break;
  }
  os << '[';
  if (!condition().tautology()) {
    condition().print(os, predicates, terms, 0, Bindings());
  }
  os << ',';
  if (!link_condition().tautology()) {
    link_condition().print(os, predicates, terms, 0, Bindings());
  }
  os << "->";
  if (add_list().size() + del_list().size() == 1) {
    if (!add_list().empty()) {
      add_list().front()->print(os, predicates, terms, 0, Bindings());
    } else {
      del_list().front()->print(os, predicates, terms, 0, Bindings());
    }
  } else {
    os << "(and";
    for (AtomListIter ai = add_list().begin(); ai != add_list().end(); ai++) {
      os << ' ';
      (*ai)->print(os, predicates, terms, 0, Bindings());
    }
    for (NegationListIter ni = del_list().begin();
	 ni != del_list().end(); ni++) {
      os << ' ';
      (*ni)->print(os, predicates, terms, 0, Bindings());
    }
    os << ")";
  }
  os << ']' << ')';
}


/* ====================================================================== */
/* Action */

/* Constructs an action with the given name. */
Action::Action(const std::string& name, bool durative)
  : name_(name), condition_(&Condition::TRUE), durative_(durative),
    min_duration_(0.0f), max_duration_(durative ? INFINITY : 0.0f) {
  Condition::register_use(condition_);
}


/* Deletes this action. */
Action::~Action() {
  Condition::unregister_use(condition_);
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    delete *ei;
  }
}


/* Sets the condition for this action. */
void Action::set_condition(const Condition& condition) {
  if (condition_ != &condition) {
    Condition::unregister_use(condition_);
    condition_ = &condition;
    Condition::register_use(condition_);
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
	for (VariableList::const_iterator vi = ei.forall().begin();
	     vi != ei.forall().end(); vi++) {
	  one_eff.add_forall(*vi);
	}
	one_eff.set_condition(ei.condition());
	one_eff.add_positive(**ai);
	effects_.push_back(&one_eff);
      }
      for (NegationListIter ni = ei.del_list().begin();
	   ni != ei.del_list().end(); ni++) {
	Effect& one_eff = *(new Effect(ei.when()));
	for (VariableList::const_iterator vi = ei.forall().begin();
	     vi != ei.forall().end(); vi++) {
	  one_eff.add_forall(*vi);
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
	  BindingList mgu;
	  if (Bindings::unifiable(mgu, neg.atom(), 0, atom, 0)) {
	    const Formula* sep = &Formula::FALSE;
	    for (BindingList::const_iterator si = mgu.begin();
		 si != mgu.end(); si++) {
	      const Binding& subst = *si;
	      Variable var = subst.var();
	      if (find(ej.forall().begin(), ej.forall().end(), var)
		  == ej.forall().end()) {
		if (is_variable(subst.term())
		    || (find(ej.forall().begin(), ej.forall().end(),
			     subst.term()) == ej.forall().end())) {
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
	if (ei.when() == Effect::AT_START) {
	  ei.set_link_condition(Condition::make_condition(*cond, AT_START));
	} else {
	  ei.set_link_condition(Condition::make_condition(*cond, AT_END));
	}
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
    const Formula* cond = &condition().over_all().separator(*literal);
    ei.set_link_condition(ei.link_condition()
			  && Condition::make_condition(*cond, OVER_ALL));
    if (ei.when() != Effect::AT_END) {
      cond = &condition().at_start().separator(*literal);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make_condition(*cond, AT_START));
    }
    if (ei.when() != Effect::AT_START) {
      cond = &condition().at_end().separator(*literal);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make_condition(*cond, AT_END));
    }
  }
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Adds a parameter to this action schema. */
void ActionSchema::add_parameter(Variable var) {
  parameters_.push_back(var);
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
      instantiation(SubstitutionMap(), problem, condition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    SubstitutionMap args;
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.domain().terms().type(parameters()[i]));
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const Condition*> conds;
    conds.push(&condition());
    Condition::register_use(conds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Condition& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Condition::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
	if (!inst_cond.contradiction()) {
	  const GroundAction* inst_action =
	    instantiation(args, problem, inst_cond);
	  if (inst_action != NULL) {
	    actions.push_back(inst_action);
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Condition::unregister_use(conds.top());
	  conds.pop();
	  args.erase(parameters()[j]);
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
      Condition::unregister_use(conds.top());
      conds.pop();
    }
  }
}


/* Returns an instantiation of this action schema. */
const GroundAction*
ActionSchema::instantiation(const SubstitutionMap& args,
			    const Problem& problem,
			    const Condition& condition) const {
  EffectList inst_effects;
  size_t useful = 0;
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    (*ei)->instantiations(inst_effects, useful, args, problem);
  }
  if (useful > 0) {
    GroundAction& ga = *(new GroundAction(name(), durative()));
    size_t n = parameters().size();
    for (size_t i = 0; i < n; i++) {
      SubstitutionMap::const_iterator si = args.find(parameters()[i]);
      ga.add_argument((*si).second);
    }
    ga.set_condition(condition);
    for (EffectListIter ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(min_duration());
    ga.set_max_duration(max_duration());
    return &ga;
  } else {
    for (EffectListIter ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      delete *ei;
    }
    return NULL;
  }
}


/* Prints this action on the given stream. */
void ActionSchema::print(std::ostream& os, const PredicateTable& predicates,
			 const TermTable& terms) const {
  os << '(' << name() << " (";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    terms.print_term(os, *vi);
  }
  os << ") ";
  if (!condition().tautology()) {
    condition().print(os, predicates, terms, 0, Bindings());
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects().begin(); ei != effects().end(); ei++) {
    if (ei != effects().begin()) {
      os << ' ';
    }
    (*ei)->print(os, predicates, terms);
  }
  os << ")" << ')';
}


/* Prints this action on the given stream with the given bindings. */
void ActionSchema::print(std::ostream& os, const TermTable& terms,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (VariableList::const_iterator ti = parameters().begin();
       ti != parameters().end(); ti++) {
    os << ' ';
    terms.print_term(os, *ti, step_id, bindings);
  }
  os << ')';
}


/* ====================================================================== */
/* GroundAction */

/* Constructs a ground action with the given name. */
GroundAction::GroundAction(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Adds an argument to this ground action. */
void GroundAction::add_argument(Object arg) {
  arguments_.push_back(arg);
}


/* Prints this action on the given stream with the given bindings. */
void GroundAction::print(std::ostream& os, const TermTable& terms,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (ObjectList::const_iterator ni = arguments().begin();
       ni != arguments().end(); ni++) {
    os << ' ';
    terms.print_term(os, *ni, step_id, bindings);
  }
  os << ')';
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
}


/* Domain actions. */
const ActionSchemaMap& Domain::actions() const {
  return actions_;
}


/* Adds an action to this domain. */
void Domain::add_action(const ActionSchema& action) {
  actions_.insert(make_pair(action.name(), &action));
  PredicateSet achievable_preds;
  action.achievable_predicates(achievable_preds, achievable_preds);
  for (PredicateSet::const_iterator pi = achievable_preds.begin();
       pi != achievable_preds.end(); pi++) {
    predicates().make_dynamic(*pi);
  }
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const std::string& name) const {
  ActionSchemaMapIter ai = actions_.find(name);
  return (ai != actions_.end()) ? (*ai).second : NULL;
}


/* Fills the provided object list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(ObjectList& constants, Type type) const {
  Object last = terms().last_object();
  for (Object i = terms().first_object(); i <= last; i++) {
    if (types().subtype(terms().type(i), type)) {
      constants.push_back(i);
    }
  }
}


/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << std::endl << "types:";
  for (Type i = d.types().first_type(); i < d.types().last_type(); i++) {
    os << std::endl << "  ";
    d.types().print_type(os, i);
    bool first = true;
    for (Type j = d.types().first_type(); j < d.types().last_type(); j++) {
      if (i != j && d.types().subtype(i, j)) {
	if (first) {
	  os << " <:";
	  first = false;
	}
	os << ' ';
	d.types().print_type(os, j);
      }
    }
  }
  os << std::endl << "predicates:";
  for (Predicate i = d.predicates().first_predicate();
       i < d.predicates().last_predicate(); i++) {
    os << std::endl << "  (" << d.predicates().name(i);
    size_t arity = d.predicates().arity(i);
    for (size_t j = 0; j < arity; j++) {
      os << " ?x - ";
      d.types().print_type(os, d.predicates().parameter(i, j));
    }
    os << ")";
    if (d.predicates().static_predicate(i)) {
      os << " <static>";
    }
  }
  os << std::endl << "constants:";
  for (Object i = d.terms().first_object();
       i <= d.terms().last_object(); i++) {
    os << std::endl << "  ";
    d.terms().print_term(os, i, 0, Bindings());
    os << " - ";
    d.types().print_type(os, d.terms().type(i));
  }
  os << std::endl << "actions:";
  for (ActionSchemaMapIter ai = d.actions_.begin();
       ai != d.actions_.end(); ai++) {
    os << std::endl << "  ";
    (*ai).second->print(os, d.predicates(), d.terms());
  }
  return os;
}
