/*
 * $Id: domains.cc,v 1.2 2001-05-04 04:45:28 lorens Exp $
 */
#include "domains.h"


/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Prints this predicate on the given stream. */
void Predicate::print(ostream& os) const {
  os << '(' << name;
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    os << ' ' << **i;
    if (!(*i)->type.object()) {
      os << " - " << (*i)->type;
    }
  }
  os << ')';
}


/* Prints this effect on the given stream. */
void Effect::print(ostream& os) const {
  os << '[';
  if (condition != NULL) {
    os << *condition;
  }
  os << "->";
  if (add_list.size() == 1) {
    os << *add_list.front();
  } else {
    os << "(and";
    for (FormulaList::const_iterator i = add_list.begin();
	 i != add_list.end(); i++) {
      os << ' ' << **i;
    }
    os << ")";
  }
  os << ']';
}


/* Prints this action on the given stream. */
void Action::print(ostream& os) const {
  os << '(' << name << " (";
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    if (i != parameters.begin()) {
      os << ' ';
    }
    os << **i;
    if (!(*i)->type.object()) {
      os << " - " << (*i)->type;
    }
  }
  os << ") ";
  if (precondition != NULL) {
    os << *precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectList::const_iterator i = effects.begin();
       i != effects.end(); i++) {
    if (i != effects.begin()) {
      os << ' ';
    }
    os << **i;
  }
  os << ")" << ')';
}


/* Prints this domain on the given stream. */
void Domain::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "action style: "
     << ((action_style == STRIPS) ? "STRIPS" : "ADL");
  os << endl << "types:";
  for (TypeMap::const_iterator i = types_.begin(); i != types_.end(); i++) {
    if (!(*i).second->object()) {
      os << ' ' << *(*i).second;
      if (!(*i).second->super_type.object()) {
	os << " - " << (*i).second->super_type;
      }
    }
  }
  os << endl << "constants:";
  for (NameMap::const_iterator i = constants_.begin();
       i != constants_.end(); i++) {
    os << ' ' << *(*i).second;
    if (!(*i).second->type.object()) {
      os << " - " << (*i).second->type;
    }
  }
  os << endl << "predicates:";
  for (PredicateMap::const_iterator i = predicates_.begin();
       i != predicates_.end(); i++) {
    os << endl << "  " << *(*i).second;
  }
  os << endl << "actions:";
  for (ActionMap::const_iterator i = actions_.begin();
       i != actions_.end(); i++) {
    os << endl << "  " << *(*i).second;
  }
}
