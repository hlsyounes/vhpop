/*
 * $Id: problems.cc,v 1.4 2001-10-06 23:28:57 lorens Exp $
 */
#include "problems.h"
#include "domains.h"
#include "formulas.h"


/* Table of defined problems. */
Problem::ProblemMap Problem::problems = Problem::ProblemMap();


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
void Problem::instantiated_actions(ActionList& actions) const {
  for (ActionMapIter ai = domain.actions.begin();
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
  if (init != NULL) {
    os << endl << "initial condition: " << *init;
  }
  os << endl << "goal: " << goal;
}
