/*
 * $Id: formulas.cc,v 1.1 2001-05-03 15:15:08 lorens Exp $
 */
#include "formulas.h"


/* The object type. */
const SimpleType& SimpleType::OBJECT_TYPE = *(new SimpleType("object"));


/* Prints this type on the given stream. */
void SimpleType::print(ostream& os) const {
  os << name;
}


/* Prints this type on the given stream. */
void UnionType::print(ostream& os) const {
  os << "(either";
  for (TypeList::const_iterator i = types.begin(); i != types.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}


/* Prints this term on the given stream. */
void Term::print(ostream& os) const {
  os << name;
}


/* Prints this instantiated variable on the given stream. */
void StepVar::print(ostream& os) const {
  os << name << '(' << id_ << ')';
}


/* Prints this atomic formula on the given stream. */
void AtomicFormula::print(ostream& os) const {
  os << '(' << predicate;
  for (TermList::const_iterator i = terms.begin(); i != terms.end(); i++) {
    os << ' ' << **i;
  }
  os << ')';
}


/* Prints this negation on the given stream. */
void Negation::print(ostream& os) const {
  os << "(not " << atom << ")";
}


/* Prints this equality on the given stream. */
void Equality::print(ostream& os) const {
  os << "(= " << term1 << ' ' << term2 << ")";
}


/* Prints this inequality on the given stream. */
void Inequality::print(ostream& os) const {
  os << "(not (= " << term1 << ' ' << term2 << "))";
}


/* Prints this conjunction on the given stream. */
void Conjunction::print(ostream& os) const {
  os << "(and";
  for (FormulaList::const_iterator i = conjuncts.begin();
       i != conjuncts.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}


/* Prints this disjunction on the given stream. */
void Disjunction::print(ostream& os) const {
  os << "(or";
  for (FormulaList::const_iterator i = disjuncts.begin();
       i != disjuncts.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}


/* Prints this formula on the given stream. */
void ExistsFormula::print(ostream& os) const {
  os << "(exists (";
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    if (i != parameters.begin()) {
      os << ' ';
    }
    os << **i;
    if (&(*i)->type != &SimpleType::OBJECT_TYPE) {
      os << " - " << (*i)->type;
    }
  }
  os << ") " << body << ")";
}


/* Prints this formula on the given stream. */
void ForallFormula::print(ostream& os) const {
  os << "(forall (";
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    if (i != parameters.begin()) {
      os << ' ';
    }
    os << **i;
    if (&(*i)->type != &SimpleType::OBJECT_TYPE) {
      os << " - " << (*i)->type;
    }
  }
  os << ") " << body << ")";
}
