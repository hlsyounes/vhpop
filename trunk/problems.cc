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
 * $Id: problems.cc,v 6.7 2003-09-05 16:31:04 lorens Exp $
 */
#include "problems.h"
#include "domains.h"
#include "bindings.h"
#include <iostream>


/* ====================================================================== */
/* Problem */

/* Table of defined problems. */
Problem::ProblemMap Problem::problems = Problem::ProblemMap();


/* Returns a const_iterator pointing to the first problem. */
Problem::ProblemMap::const_iterator Problem::begin() {
  return problems.begin();
}


/* Returns a const_iterator pointing beyond the last problem. */
Problem::ProblemMap::const_iterator Problem::end() {
  return problems.end();
}


/* Returns the problem with the given name, or NULL if it is undefined. */
const Problem* Problem::find(const std::string& name) {
  ProblemMap::const_iterator pi = problems.find(name);
  return (pi != problems.end()) ? (*pi).second : NULL;
}


/* Removes all defined problems. */
void Problem::clear() {
  ProblemMap::const_iterator pi = begin();
  while (pi != end()) {
    delete (*pi).second;
    pi = begin();
  }
  problems.clear();
}


/* Constructs a problem. */
Problem::Problem(const std::string& name, const Domain& domain)
  : name_(name), domain_(&domain), terms_(TermTable(domain.terms())),
    init_action_(GroundAction("", false)), goal_(&Formula::TRUE),
    metric_(new Value(0)) {
  Formula::register_use(goal_);
  Expression::register_use(metric_);
  const Problem* p = find(name);
  if (p != NULL) {
    delete p;
  }
  problems[name] = this;
}


/* Deletes a problem. */
Problem::~Problem() {
  problems.erase(name());
  for (ValueMap::const_iterator vi = init_values_.begin();
       vi != init_values_.end(); vi++) {
    Expression::unregister_use((*vi).first);
  }
  Formula::unregister_use(goal_);
  Expression::unregister_use(metric_);
  for (std::map<Type, const ObjectList*>::const_iterator oi =
	 compatible_.begin(); oi != compatible_.end(); oi++) {
    delete (*oi).second;
  }
  compatible_.clear();
}


/* Adds an atomic formula to the initial conditions of this problem. */
void Problem::add_init_atom(const Atom& atom) {
  init_atoms_.insert(&atom);
  init_action_.add_effect(*new Effect(atom, Effect::AT_END));
}


/* Adds a function application value to the initial conditions of
   this problem. */
void Problem::add_init_value(const Application& application, float value) {
  if (init_values_.find(&application) == init_values_.end()) {
    init_values_.insert(std::make_pair(&application, value));
    Expression::register_use(&application);
  } else {
    init_values_[&application] = value;
  }
}


/* Sets the goal of this problem. */
void Problem::set_goal(const Formula& goal) {
  if (goal_ != &goal) {
    Formula::unregister_use(goal_);
    goal_ = &goal;
    Formula::register_use(goal_);
  }
}


/* Sets the metric to minimize for this problem. */
void Problem::set_metric(const Expression& metric, bool negate) {
  const Expression* real_metric;
  if (negate) {
    real_metric = &Subtraction::make(*new Value(0), metric);
  } else {
    real_metric = &metric;
  }
  const Expression& inst_metric =
    real_metric->instantiation(SubstitutionMap(), *this);
  if (&inst_metric != real_metric) {
    Expression::register_use(real_metric);
    Expression::unregister_use(real_metric);
    real_metric = &inst_metric;
  }
  if (real_metric != metric_) {
    Expression::unregister_use(metric_);
    metric_ = real_metric;
    Expression::register_use(metric_);
  }
}


/* Tests if the metric is constant. */
bool Problem::constant_metric() const {
  return typeid(metric()) == typeid(Value);
}


/* Returns a list with objects (including constants declared in the
   domain) that are compatible with the given type. */
const ObjectList& Problem::compatible_objects(Type type) const {
  std::map<Type, const ObjectList*>::const_iterator oi =
    compatible_.find(type);
  if (oi != compatible_.end()) {
    return *(*oi).second;
  } else {
    ObjectList& objects = *new ObjectList();
    domain().compatible_constants(objects, type);
    Object last = terms().last_object();
    for (Object i = terms().first_object(); i <= last; i++) {
      if (domain().types().subtype(terms().type(i), type)) {
	objects.push_back(i);
      }
    }
    compatible_.insert(std::make_pair(type, &objects));
    return objects;
  }
}


/* Returns a new variable for this problem. */
Variable Problem::new_variable(Type type) const {
  return terms_.add_variable(type);
}


/* Fills the provided action list with ground actions instantiated
   from the action schemas of the domain. */
void Problem::instantiated_actions(GroundActionList& actions) const {
  for (ActionSchemaMap::const_iterator ai = domain().actions().begin();
       ai != domain().actions().end(); ai++) {
    (*ai).second->instantiations(actions, *this);
  }
  for (std::map<Type, const ObjectList*>::const_iterator oi =
	 compatible_.begin(); oi != compatible_.end(); oi++) {
    delete (*oi).second;
  }
  compatible_.clear();
}


/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p) {
  os << "name: " << p.name();
  os << std::endl << "domain: " << p.domain().name();
  os << std::endl << "objects:";
  for (Object i = p.terms().first_object();
       i <= p.terms().last_object(); i++) {
    os << std::endl << "  ";
    p.terms().print_term(os, i);
    os << " - ";
    p.domain().types().print_type(os, p.terms().type(i));
  }
  os << std::endl << "init: ";
  AtomSet::const_iterator ai = p.init_atoms().begin();
  (*ai)->print(os, p.domain().predicates(), p.terms(), 0, Bindings::EMPTY);
  for (ai++; ai != p.init_atoms().end(); ai++) {
    os << ' ';
    (*ai)->print(os, p.domain().predicates(), p.terms(), 0, Bindings::EMPTY);
  }
  for (ValueMap::const_iterator vi = p.init_values_.begin();
       vi != p.init_values_.end(); vi++) {
    os << std::endl << "  (= ";
    (*vi).first->print(os, p.domain().functions(), p.terms());
    os << ' ' << (*vi).second << ")";
  }
  os << std::endl << "goal: ";
  p.goal().print(os, p.domain().predicates(), p.terms(), 0, Bindings::EMPTY);
  os << std::endl << "metric: ";
  p.metric().print(os, p.domain().functions(), p.terms());
  return os;
}
