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
 * $Id: problems.cc,v 3.2 2002-03-18 09:31:37 lorens Exp $
 */
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"


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
const Problem* Problem::find(const string& name) {
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
Problem::Problem(const string& name, const Domain& domain,
		 const NameMap& objects, const Effect& init,
		 const Formula& goal)
  : name(name), domain(domain), objects(objects), init(init), goal(goal) {
  const Problem* p = find(name);
  if (p != NULL) {
    delete p;
  }
  problems[name] = this;
}


/* Deletes a problem. */
Problem::~Problem() {
  problems.erase(name);
  for (NameMapIter ni = objects.begin(); ni != objects.end(); ni++) {
    delete (*ni).second;
  }
  delete &objects;
}


/* Returns the object with the given name, or NULL if it is
   undefined. */
const Name* Problem::find_object(const string& name) const {
  NameMapIter ni = objects.find(name);
  return (ni != objects.end()) ? (*ni).second : NULL;
}


/* Fills the provided name list with objects (including constants
   declared in the domain) that are compatible with the given type. */
void Problem::compatible_objects(NameList& objects, const Type& t) const {
  domain.compatible_constants(objects, t);
  for (NameMapIter ni = this->objects.begin();
       ni != this->objects.end(); ni++) {
    const Name& name = *(*ni).second;
    if (name.type.subtype(t)) {
      objects.push_back(&name);
    }
  }
}


/* Fills the provided action list with ground actions instantiated
   from the action schemas of the domain. */
void Problem::instantiated_actions(GroundActionList& actions) const {
  for (ActionSchemaMapIter ai = domain.actions.begin();
       ai != domain.actions.end(); ai++) {
    (*ai).second->instantiations(actions, *this);
  }
}


/* Prints this object on the given stream. */
void Problem::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "domain: " << domain.name;
  os << endl << "objects:";
  for (NameMapIter ni = objects.begin(); ni != objects.end(); ni++) {
    os << ' ' << *(*ni).second;
    if (!(*ni).second->type.object()) {
      os << " - " << (*ni).second->type;
    }
  }
  os << endl << "initial condition: " << init;
  os << endl << "goal: " << goal;
}
