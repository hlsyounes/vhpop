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
 * $Id: domains.cc,v 6.4 2003-07-28 21:34:03 lorens Exp $
 */
#include "domains.h"
#include "bindings.h"
#include "problems.h"
#include "mathport.h"
#include <stack>
#include <iostream>


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
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
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
    for (EffectList::const_iterator ei = effects_.begin();
	 ei != effects_.end(); ei++) {
      const Literal& literal = (*ei)->literal();
      if (typeid(literal) == typeid(Atom)) {
	preds.insert(literal.predicate());
      } else {
	neg_preds.insert(literal.predicate());
      }
    }
  }
}


/* "Strengthens" the effects of this action. */
void Action::strengthen_effects() {
  /*
   * Separate negative effects from positive effects occuring at the
   * same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    if (typeid(ei.literal()) == typeid(Negation)) {
      const Negation& neg = dynamic_cast<const Negation&>(ei.literal());
      const Condition* cond = &Condition::TRUE;
      for (EffectList::const_iterator j = effects_.begin();
	   j != effects_.end() && !cond->contradiction(); j++) {
	const Effect& ej = **j;
	if (ei.when() == ej.when() && typeid(ej.literal()) == typeid(Atom)) {
	  bool diff_param = ei.arity() != ej.arity();
	  for (size_t pi = 0; pi < ei.arity() && !diff_param; pi++) {
	    if (ei.parameter(pi) != ej.parameter(pi)) {
	      diff_param = true;
	    }
	  }
	  if (!diff_param) {
	    /* Only separate two effects with same universally
	       quantified variables. */
	    BindingList mgu;
	    if (Bindings::unifiable(mgu, neg.atom(), 0, ej.literal(), 0)) {
	      const Formula* sep = &Formula::FALSE;
	      for (BindingList::const_iterator si = mgu.begin();
		   si != mgu.end(); si++) {
		const Binding& subst = *si;
		sep = &(*sep || Inequality::make(subst.var(), subst.term()));
	      }
	      cond = &(*cond && (Condition::make(*sep, AT_START)
				 || !ej.condition()));
	    }
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
    const Literal& literal = ei.literal();
    const Formula* cond = &condition().over_all().separator(literal);
    ei.set_link_condition(ei.link_condition()
			  && Condition::make(*cond, OVER_ALL));
    if (ei.when() != Effect::AT_END) {
      cond = &condition().at_start().separator(literal);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make(*cond, AT_START));
    }
    if (ei.when() != Effect::AT_START) {
      cond = &condition().at_end().separator(literal);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make(*cond, AT_END));
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
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    (*ei)->instantiations(inst_effects, useful, args, problem);
  }
  if (useful > 0) {
    GroundAction& ga = *new GroundAction(name(), durative());
    size_t n = parameters().size();
    for (size_t i = 0; i < n; i++) {
      SubstitutionMap::const_iterator si = args.find(parameters()[i]);
      ga.add_argument((*si).second);
    }
    ga.set_condition(condition);
    for (EffectList::const_iterator ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(min_duration());
    ga.set_max_duration(max_duration());
    return &ga;
  } else {
    for (EffectList::const_iterator ei = inst_effects.begin();
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
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
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
