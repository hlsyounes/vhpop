/*
 * Copyright (C) 2002-2004 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: problems.cc,v 6.9 2004-02-07 11:32:57 lorens Exp $
 */
#include "problems.h"
#include "domains.h"
#include "bindings.h"
#include <iostream>
#include <typeinfo>
#if HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
namespace std {
  typedef std::ostrstream ostringstream;
}
#endif


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
    init_action_(GroundAction("<init 0>", false)), goal_(&Formula::TRUE),
    metric_(new Value(0)) {
  Formula::register_use(goal_);
  RCObject::ref(metric_);
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
    RCObject::destructive_deref((*vi).first);
  }
  for (TimedActionTable::const_iterator ai = timed_actions_.begin();
       ai != timed_actions_.end(); ai++) {
    delete (*ai).second;
  }
  Formula::unregister_use(goal_);
  RCObject::destructive_deref(metric_);
}


/* Adds an atomic formula to the initial conditions of this problem. */
void Problem::add_init_atom(const Atom& atom) {
  init_atoms_.insert(&atom);
  init_action_.add_effect(*new Effect(atom, Effect::AT_END));
}


/* Adds a timed initial literal to this problem. */
void Problem::add_init_literal(float time, const Literal& literal) {
  if (time == 0.0f) {
    const Atom* atom = dynamic_cast<const Atom*>(&literal);
    if (atom != NULL) {
      add_init_atom(*atom);
    }
  } else {
    GroundAction* action;
    TimedActionTable::const_iterator ai = timed_actions_.find(time);
    if (ai != timed_actions_.end()) {
      action = (*ai).second;
    } else {
      std::ostringstream ss;
      ss << "<init " << time << '>';
#if !HAVE_SSTREAM
      ss << '\0';
#endif
      action = new GroundAction(ss.str(), false);
      timed_actions_.insert(std::make_pair(time, action));
    }
    action->add_effect(*new Effect(literal, Effect::AT_END));
  }
}


/* Adds a fluent value to the initial conditions of this problem. */
void Problem::add_init_value(const Fluent& fluent, float value) {
  if (init_values_.find(&fluent) == init_values_.end()) {
    init_values_.insert(std::make_pair(&fluent, value));
    RCObject::ref(&fluent);
  } else {
    init_values_[&fluent] = value;
  }
}


/* Sets the goal of this problem. */
void Problem::set_goal(const Formula& goal) {
  if (goal_ != &goal) {
    Formula::register_use(&goal);
    Formula::unregister_use(goal_);
    goal_ = &goal;
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
    real_metric->instantiation(SubstitutionMap(), init_values());
  if (&inst_metric != real_metric) {
    RCObject::ref(real_metric);
    RCObject::destructive_deref(real_metric);
    real_metric = &inst_metric;
  }
  if (real_metric != metric_) {
    RCObject::ref(real_metric);
    RCObject::destructive_deref(metric_);
    metric_ = real_metric;
  }
}


/* Tests if the metric is constant. */
bool Problem::constant_metric() const {
  return typeid(metric()) == typeid(Value);
}


/* Fills the provided action list with ground actions instantiated
   from the action schemas of the domain. */
void Problem::instantiated_actions(GroundActionList& actions) const {
  for (ActionSchemaMap::const_iterator ai = domain().actions().begin();
       ai != domain().actions().end(); ai++) {
    (*ai).second->instantiations(actions, *this);
  }
}


/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p) {
  os << "name: " << p.name();
  os << std::endl << "domain: " << p.domain().name();
  os << std::endl << "objects:" << p.terms();
  os << std::endl << "init:";
  for (AtomSet::const_iterator ai = p.init_atoms().begin();
       ai != p.init_atoms().end(); ai++) {
    os << ' ';
    (*ai)->print(os, 0, Bindings::EMPTY);
  }
  for (TimedActionTable::const_iterator ai = p.timed_actions().begin();
       ai != p.timed_actions().end(); ai++) {
    float time = (*ai).first;
    const EffectList& effects = (*ai).second->effects();
    for (EffectList::const_iterator ei = effects.begin();
	 ei != effects.end(); ei++) {
      os << " (at " << time << ' ';
      (*ei)->literal().print(os, 0, Bindings::EMPTY);
      os << ")";
    }
  }
  for (ValueMap::const_iterator vi = p.init_values_.begin();
       vi != p.init_values_.end(); vi++) {
    os << std::endl << "  (= " << *(*vi).first << ' ' << (*vi).second << ")";
  }
  os << std::endl << "goal: ";
  p.goal().print(os, 0, Bindings::EMPTY);
  os << std::endl << "metric: " << p.metric();
  return os;
}
