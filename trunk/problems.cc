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
 * $Id: problems.cc,v 6.2 2003-07-21 02:24:53 lorens Exp $
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
Problem::ProblemMapIter Problem::begin() {
  return problems.begin();
}


/* Returns a const_iterator pointing beyond the last problem. */
Problem::ProblemMapIter Problem::end() {
  return problems.end();
}


/* Returns the problem with the given name, or NULL if it is undefined. */
const Problem* Problem::find(const std::string& name) {
  ProblemMapIter pi = problems.find(name);
  return (pi != problems.end()) ? (*pi).second : NULL;
}


/* Removes all defined problems. */
void Problem::clear() {
  ProblemMapIter pi = begin();
  while (pi != end()) {
    delete (*pi).second;
    pi = begin();
  }
  problems.clear();
}


/* Constructs a problem. */
Problem::Problem(const std::string& name, const Domain& domain)
  : name_(name), domain_(&domain), terms_(TermTable(domain.terms())),
    init_(new Effect(Effect::AT_END)),
    init_action_(GroundAction("", false)),
    goal_(&Formula::TRUE) {
  init_action_.add_effect(*init_);
  Formula::register_use(goal_);
  const Problem* p = find(name);
  if (p != NULL) {
    delete p;
  }
  problems[name] = this;
}


/* Deletes a problem. */
Problem::~Problem() {
  problems.erase(name());
  Formula::unregister_use(goal_);
}


/* Adds an atomic formula to the initial conditions of this problem. */
void Problem::add_init(const Atom& atom) {
  init_->add_positive(atom);
}


/* Sets the goal of this problem. */
void Problem::set_goal(const Formula& goal) {
  if (goal_ != &goal) {
    Formula::unregister_use(goal_);
    goal_ = &goal;
    Formula::register_use(goal_);
  }
}


/* Fills the provided object list with objects (including constants
   declared in the domain) that are compatible with the given
   type. */
void Problem::compatible_objects(ObjectList& objects, Type type) const {
  domain().compatible_constants(objects, type);
  Object last = terms().last_object();
  for (Object i = terms().first_object(); i <= last; i++) {
    if (domain().types().subtype(terms().type(i), type)) {
      objects.push_back(i);
    }
  }
}


/* Returns a new variable for this problem. */
Variable Problem::new_variable(Type type) const {
  return terms_.add_variable(type);
}


/* Fills the provided action list with ground actions instantiated
   from the action schemas of the domain. */
void Problem::instantiated_actions(GroundActionList& actions) const {
  for (ActionSchemaMapIter ai = domain().actions().begin();
       ai != domain().actions().end(); ai++) {
    (*ai).second->instantiations(actions, *this);
  }
}


/* Prints the given term on the given stream with the given
   bindings. */
void Problem::print_term(std::ostream& os, Term term, size_t step_id,
			 const Bindings& bindings) const {
}


/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p) {
  os << "name: " << p.name();
  os << std::endl << "domain: " << p.domain().name();
  os << std::endl << "objects:";
  for (Object i = p.terms().first_object();
       i <= p.terms().last_object(); i++) {
    os << std::endl << "  ";
    p.terms().print_term(os, i, 0, Bindings());
    os << " - ";
    p.domain().types().print_type(os, p.terms().type(i));
  }
  os << std::endl << "initial condition: ";
  p.init().print(os, p.domain().predicates(), p.terms());
  os << std::endl << "goal: ";
  p.goal().print(os, p.domain().predicates(), p.terms(), 0, Bindings());
  return os;
}
