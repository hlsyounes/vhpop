/*
 * $Id: problems.cc,v 1.1 2001-05-03 15:27:21 lorens Exp $
 */
#include "domains.h"
#include "problems.h"


/* Table of defined problems. */
Problem::ProblemMap Problem::problems = Problem::ProblemMap();


/* Prints this problem on the given stream. */
void Problem::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "domain: " << domain.name;
  os << endl << "objects:";
  for (NameMap::const_iterator i = objects_.begin();
       i != objects_.end(); i++) {
    os << ' ' << *(*i).second;
    if (&(*i).second->type != &SimpleType::OBJECT_TYPE) {
      os << " - " << (*i).second->type;
    }
  }
  if (init != NULL) {
    os << endl << "initial condition: " << *init;
  }
  os << endl << "goal: " << goal;
}
