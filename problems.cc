/*
 * $Id: problems.cc,v 1.3 2001-08-11 06:15:30 lorens Exp $
 */
#include "problems.h"
#include "domains.h"
#include "formulas.h"


/* Table of defined problems. */
Problem::ProblemMap Problem::problems = Problem::ProblemMap();


/* Returns the object with the given name, or NULL if it is
   undefined. */
const Name* Problem::find_object(const string& name) const {
  NameMap::const_iterator i = objects.find(name);
  return (i != objects.end()) ? (*i).second : NULL;
}


/* Fills the provided name list with objects (including constants
   declared in the domain) that are compatible with the given type. */
void Problem::compatible_objects(NameList& objects, const Type& t) const {
  domain.compatible_constants(objects, t);
  for (NameMap::const_iterator i = this->objects.begin();
       i != this->objects.end(); i++) {
    const Name& name = *(*i).second;
    if (name.type.subtype(t)) {
      objects.push_back(&name);
    }
  }
}


/* Fills the provided action list with ground actions instantiated
   from the action schemas of the domain. */
void Problem::instantiated_actions(ActionList& actions) const {
  for (ActionMap::const_iterator i = domain.actions.begin();
       i != domain.actions.end(); i++) {
    (*i).second->instantiations(actions, *this);
  }
}


/* Prints this problem on the given stream. */
void Problem::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "domain: " << domain.name;
  os << endl << "objects:";
  for (NameMap::const_iterator i = objects.begin(); i != objects.end(); i++) {
    os << ' ' << *(*i).second;
    if (!(*i).second->type.object()) {
      os << " - " << (*i).second->type;
    }
  }
  if (init != NULL) {
    os << endl << "initial condition: " << *init;
  }
  os << endl << "goal: " << goal;
}
