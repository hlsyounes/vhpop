/*
 * $Id: formulas.cc,v 1.2 2001-07-29 17:55:36 lorens Exp $
 */
#include "formulas.h"
#include "domains.h"
#include "problems.h"


/* Prints this term on the given stream. */
void Term::print(ostream& os) const {
  os << name;
}


/* Prints this substitution on the given stream. */
void Substitution::print(ostream& os) const {
  os << '[' << var << '/' << term << ']';
}


/* Prints this instantiated variable on the given stream. */
void StepVar::print(ostream& os) const {
  os << name << '(' << id_ << ')';
}


/* Checks if this formula is consistent. */
bool AtomicFormula::consistent(const Problem& problem) const {
  if (problem.domain.static_predicate(predicate)) {
    if (problem.init != NULL) {
      const FormulaList& adds = problem.init->add_list;
      for (FormulaList::const_iterator i = adds.begin();
	   i != adds.end(); i++) {
	/* N.B. assumes this is a ground atomic formula (needs to
           unify otherwise) */
	if (*this == **i) {
	  return true;
	}
      }
      return false;
    }
  }
  return true;
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
