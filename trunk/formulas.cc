/*
 * $Id: formulas.cc,v 1.25 2001-10-30 16:02:05 lorens Exp $
 */
#include <typeinfo>
#include "formulas.h"
#include "domains.h"
#include "problems.h"
#include "bindings.h"


/*
 * A substitutes binary predicate.
 */
struct Substitutes
  : public binary_function<const Substitution*, const Variable*, bool> {
  /* Checks if the given substitution involves the given variable. */
  bool operator()(const Substitution* s, const Variable* v) const {
    return s->var == *v;
  }
};


/* Checks if this object equals the given object. */
bool StepVar::equals(const EqualityComparable& o) const {
  const StepVar* vt = dynamic_cast<const StepVar*>(&o);
  return vt != NULL && name == vt->name && id == vt->id;
}


/* Prints this object on the given stream. */
void StepVar::print(ostream& os) const {
  os << name << '(' << id << ')';
}


/*
 * Class representing the true formula.
 */
struct TrueFormula : public Formula {
  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
    return *this;
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const {
    return *this;
  }

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const {
    return *this;
  }

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b) const {
    return HeuristicValue::ZERO;
  }

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const {
    return equals(f);
  }

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const {
    return this == &o;
  }

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const {
    os << "TRUE";
  }

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const {
    return FALSE;
  }

private:
  TrueFormula() {
  }

  friend struct Formula;
};


/*
 * Class representing the false formula.
 */
struct FalseFormula : public Formula {
  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
    return *this;
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const {
    return *this;
  }

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const {
    return *this;
  }

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b) const {
    return HeuristicValue::INFINITE;
  }

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const {
    return equals(f);
  }

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const {
    return this == &o;
  }

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const {
    os << "FALSE";
  }

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const {
    return TRUE;
  }

private:
  FalseFormula() {
  }

  friend struct Formula;
};


/* Prints this object on the given stream. */
void Substitution::print(ostream& os) const {
  os << '[' << var << '/' << term << ']';
}


/* Returns an instantiation of this term. */
const Term& Term::instantiation(const Bindings& bindings) const {
  return bindings.binding(*this);
}


/* Checks if this object is less than the given object. */
bool Term::less(const LessThanComparable& o) const {
  const Term* t = dynamic_cast<const Term*>(&o);
  return t != NULL && name < t->name;
}


/* Returns the hash value of this object. */
size_t Term::hash_value() const {
  return hash<string>()(name);
}


/* Prints this object on the given stream. */
void Term::print(ostream& os) const {
  os << name;
}


/* Returns an instantiation of this term. */
const Name& Name::instantiation(size_t id) const {
  return *this;
}


/* Returns this term subject to the given substitutions. */
const Name& Name::substitution(const SubstitutionList& subst) const {
  return *this;
}


/* Checks if this term is equivalent to the given term.  Two terms
   are equivalent if they are the same name, or if they are both
   variables. */
bool Name::equivalent(const Term& t) const {
  return equals(t);
}


/* Checks if this object equals the given object. */
bool Name::equals(const EqualityComparable& o) const {
  const Name* nt = dynamic_cast<const Name*>(&o);
  return nt != NULL && name == nt->name;
}


/* Returns an instantiation of this term. */
const Variable& Variable::instantiation(size_t id) const {
  return *(new StepVar(*this, id));
}


/* Returns this term subject to the given substitutions. */
const Term& Variable::substitution(const SubstitutionList& subst) const {
  SubstListIter si = find_if(subst.begin(), subst.end(),
			     bind2nd(Substitutes(), this));
  return (si != subst.end()) ? (*si)->term : *this;
}


/* Checks if this term is equivalent to the given term.  Two terms
   are equivalent if they are the same name, or if they are both
   variables. */
bool Variable::equivalent(const Term& t) const {
  return dynamic_cast<const Variable*>(&t) != NULL;
}


/* Checks if this object equals the given object. */
bool Variable::equals(const EqualityComparable& o) const {
  const Variable* vt = dynamic_cast<const Variable*>(&o);
  return vt != NULL && name == vt->name && typeid(o) == typeid(Variable);
}


/* Returns an instantiation of this term list. */
const TermList& TermList::instantiation(size_t id) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->instantiation(id));
  }
  return terms;
}


/* Returns an instantiation of this term list. */
const TermList& TermList::instantiation(const Bindings& bindings) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->instantiation(bindings));
  }
  return terms;
}


/* Returns this term list subject to the given substitutions. */
const TermList& TermList::substitution(const SubstitutionList& subst) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->substitution(subst));
  }
  return terms;
}


/* Checks if this term list is equivalent to the given term list. */
bool TermList::equivalent(const TermList& terms) const {
  if (size() != terms.size()) {
    return false;
  } else {
    for (const_iterator ti = begin(), tj = terms.begin();
	 ti != end(); ti++, tj++) {
      if (!(*ti)->equivalent(**tj)) {
	return false;
      }
    }
    return true;
  }
}


/* An empty variable list. */
const VariableList& VariableList::EMPTY = *(new VariableList());


/* Checks if this variable list contains the given variable. */
bool VariableList::contains(const Variable& v) const {
  for (const_iterator vi = begin(); vi != end(); vi++) {
    if (**vi == v) {
      return true;
    }
  }
  return false;
}


/* Returns an instantiation of this variable list. */
const VariableList& VariableList::instantiation(size_t id) const {
  VariableList& variables = *(new VariableList());
  for (const_iterator vi = begin(); vi != end(); vi++) {
    variables.push_back(&(*vi)->instantiation(id));
  }
  return variables;
}


/* The true formula. */
const Formula& Formula::TRUE = *(new TrueFormula());
/* The false formula. */
const Formula& Formula::FALSE = *(new FalseFormula());


/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2) {
  if (f1 == Formula::FALSE || f2 == Formula::FALSE) {
    return Formula::FALSE;
  } else if (f1 == Formula::TRUE) {
    return f2;
  } else if (f2 == Formula::TRUE) {
    return f1;
  } else {
    FormulaList& conjuncts = *(new FormulaList());
    const Conjunction* c1 = dynamic_cast<const Conjunction*>(&f1);
    if (c1 != NULL) {
      copy(c1->conjuncts.begin(), c1->conjuncts.end(),
	   back_inserter(conjuncts));
    } else {
      conjuncts.push_back(&f1);
    }
    const Conjunction* c2 = dynamic_cast<const Conjunction*>(&f2);
    if (c2 != NULL) {
      copy(c2->conjuncts.begin(), c2->conjuncts.end(),
	   back_inserter(conjuncts));
    } else {
      conjuncts.push_back(&f2);
    }
    return *(new Conjunction(conjuncts));
  }
}


/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2) {
  if (f1 == Formula::TRUE || f2 == Formula::TRUE) {
    return Formula::TRUE;
  } else if (f1 == Formula::FALSE) {
    return f2;
  } else if (f2 == Formula::FALSE) {
    return f1;
  } else {
    FormulaList& disjuncts = *(new FormulaList());
    const Disjunction* d1 = dynamic_cast<const Disjunction*>(&f1);
    if (d1 != NULL) {
      copy(d1->disjuncts.begin(), d1->disjuncts.end(),
	   back_inserter(disjuncts));
    } else {
      disjuncts.push_back(&f1);
    }
    const Disjunction* d2 = dynamic_cast<const Disjunction*>(&f2);
    if (d2 != NULL) {
      copy(d2->disjuncts.begin(), d2->disjuncts.end(),
	   back_inserter(disjuncts));
    } else {
      disjuncts.push_back(&f2);
    }
    return *(new Disjunction(disjuncts));
  }
}


bool equal_to<const Formula*>::operator()(const Formula* f1,
					  const Formula* f2) const {
  return *f1 == *f2;
}

size_t hash<const Formula*>::operator()(const Formula* f) const {
  return hash<Hashable>()(*f);
}


/* Returns an instantiation of this formula list. */
const FormulaList& FormulaList::instantiation(size_t id) const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&(*i)->instantiation(id));
  }
  return formulas;
}


/* Checks if this formula list is equivalent to the given formula list. */
bool FormulaList::equivalent(const FormulaList& formulas) const {
  if (size() != formulas.size()) {
    return false;
  } else {
    for (const_iterator i = begin(), j = formulas.begin();
	 i != end(); i++, j++) {
      if (!(*i)->equivalent(**j)) {
	return false;
      }
    }
    return true;
  }
}


/* Returns the negation of this formula list. */
const FormulaList& FormulaList::operator!() const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&!**i);
  }
  return formulas;
}


/* Returns an instantiation of this formula. */
const Atom& Atom::instantiation(size_t id) const {
  return *(new Atom(predicate, terms.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const Bindings& bindings) const {
  return *(new Atom(predicate, terms.instantiation(bindings)));
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const SubstitutionList& subst,
				   const Problem& problem) const {
  const Formula& f = substitution(subst);
  if (problem.domain.static_predicate(predicate)) {
    const AtomList& adds = problem.init.add_list;
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      if (f == **gi) {
	return TRUE;
      } else if (Bindings::unifiable(f, **gi)) {
	return f;
      }
    }
    return FALSE;
  } else {
    return f;
  }
}


/* Returns this formula subject to the given substitutions. */
const Atom& Atom::substitution(const SubstitutionList& subst) const {
  return *(new Atom(predicate, terms.substitution(subst)));
}


/* Returns this formula with static predicates assumed true. */
const Formula& Atom::strip_static(const Domain& domain) const {
  return domain.static_predicate(predicate) ? TRUE : *this;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Atom::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Atom::equivalent(const Formula& f) const {
  const Atom* atom = dynamic_cast<const Atom*>(&f);
  return (atom != NULL && predicate == atom->predicate
	  && terms.equivalent(atom->terms));
}


/* Checks if this object equals the given object. */
bool Atom::equals(const EqualityComparable& o) const {
  const Atom* atom = dynamic_cast<const Atom*>(&o);
  return (atom != NULL && predicate == atom->predicate
	  && terms.size() == atom->terms.size()
	  && equal(terms.begin(), terms.end(), atom->terms.begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Prints this object on the given stream. */
void Atom::print(ostream& os) const {
  os << '(' << predicate;
  for (TermListIter ti = terms.begin(); ti != terms.end(); ti++) {
    os << ' ' << **ti;
  }
  os << ')';
}


/* Returns the negation of this formula. */
const Formula& Atom::negation() const {
  return *(new Negation(*this));
}


/* Returns the hash value of this object. */
size_t Atom::hash_value() const {
  hash<Hashable> h;
  size_t val = hash<string>()(predicate);
  for (TermListIter ti = terms.begin(); ti != terms.end(); ti++) {
    val = 5*val + h(**ti);
  }
  return val;
}


/* Returns an instantiation of this atom list. */
const AtomList& AtomList::instantiation(size_t id) const {
  AtomList& atoms = *(new AtomList());
  for (const_iterator i = begin(); i != end(); i++) {
    atoms.push_back(&(*i)->instantiation(id));
  }
  return atoms;
}


/* Returns this atom list subject to the given substitutions. */
const AtomList& AtomList::substitution(const SubstitutionList& subst) const {
  AtomList& atoms = *(new AtomList());
  for (const_iterator i = begin(); i != end(); i++) {
    atoms.push_back(&(*i)->substitution(subst));
  }
  return atoms;
}


/* Returns an instantiation of this formula. */
const Negation& Negation::instantiation(size_t id) const {
  return *(new Negation(atom.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Negation::instantiation(const Bindings& bindings) const {
  return !atom.instantiation(bindings);
}


/* Returns an instantiation of this formula. */
const Formula& Negation::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return !atom.instantiation(subst, problem);
}


/* Returns this formula subject to the given substitutions. */
const Negation& Negation::substitution(const SubstitutionList& subst) const {
  return *(new Negation(atom.substitution(subst)));
}


/* Returns this formula with static predicates assumed true. */
const Formula& Negation::strip_static(const Domain& domain) const {
  return domain.static_predicate(atom.predicate) ? FALSE : *this;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Negation::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Negation::equivalent(const Formula& f) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  return negation != NULL && atom.equivalent(negation->atom);
}


/* Checks if this object equals the given object. */
bool Negation::equals(const EqualityComparable& o) const {
  const Negation* negation = dynamic_cast<const Negation*>(&o);
  return negation != NULL && atom == negation->atom;
}


/* Prints this object on the given stream. */
void Negation::print(ostream& os) const {
  os << "(not " << atom << ")";
}


/* Returns the negation of this formula. */
const Formula& Negation::negation() const {
  return atom;
}


/* Returns the hash value of this object. */
size_t Negation::hash_value() const {
  return 5*hash<Hashable>()(atom);
}


/* Returns an instantiation of this negation list. */
const NegationList& NegationList::instantiation(size_t id) const {
  NegationList& negations = *(new NegationList());
  for (const_iterator i = begin(); i != end(); i++) {
    negations.push_back(&(*i)->instantiation(id));
  }
  return negations;
}


/* Returns this negation list subject to the given substitutions. */
const NegationList&
NegationList::substitution(const SubstitutionList& subst) const {
  NegationList& negations = *(new NegationList());
  for (const_iterator i = begin(); i != end(); i++) {
    negations.push_back(&(*i)->substitution(subst));
  }
  return negations;
}


/* Returns the instantiation of this formula. */
const Equality& Equality::instantiation(size_t id) const {
  const Term& t1 = term1.instantiation(id);
  const Term& t2 = term2.instantiation(id);
  return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Equality(t1, t2));
}


/* Returns an instantiation of this formula. */
const Formula& Equality::instantiation(const Bindings& bindings) const {
  const Term& t1 = term1.instantiation(bindings);
  const Term& t2 = term2.instantiation(bindings);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 == t2) ? TRUE : FALSE;
  } else {
    return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Equality(t1, t2));
  }
}


/* Returns an instantiation of this formula. */
const Formula& Equality::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return substitution(subst);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Equality::substitution(const SubstitutionList& subst) const {
  const Term& t1 = term1.substitution(subst);
  const Term& t2 = term2.substitution(subst);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 == t2) ? TRUE : FALSE;
  } else {
    return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Equality(t1, t2));
  }
}


/* Returns this formula with static predicates assumed true. */
const Formula& Equality::strip_static(const Domain& domain) const {
  return *this;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Equality::strip_equality() const {
  return TRUE;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Equality::equivalent(const Formula& f) const {
  const Equality* eq = dynamic_cast<const Equality*>(&f);
  return (eq != NULL
	  && term1.equivalent(eq->term1) && term2.equivalent(eq->term2));
}


/* Checks if this object equals the given object. */
bool Equality::equals(const EqualityComparable& o) const {
  const Equality* eq = dynamic_cast<const Equality*>(&o);
  return eq != NULL && term1 == eq->term1 && term2 == eq->term2;
}


/* Prints this object on the given stream. */
void Equality::print(ostream& os) const {
  os << "(= " << term1 << ' ' << term2 << ")";
}


/* Returns the negation of this formula. */
const Formula& Equality::negation() const {
  return *(new Inequality(term1, term2));
}


/* Returns an instantiation of this formula. */
const Inequality& Inequality::instantiation(size_t id) const {
  const Term& t1 = term1.instantiation(id);
  const Term& t2 = term2.instantiation(id);
  return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Inequality(t1, t2));
}


/* Returns an instantiation of this formula. */
const Formula& Inequality::instantiation(const Bindings& bindings) const {
  const Term& t1 = term1.instantiation(bindings);
  const Term& t2 = term2.instantiation(bindings);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 != t2) ? TRUE : FALSE;
  } else {
    return ((&t1 == &term1 && &t2 == &term2)
	    ? *this : *(new Inequality(t1, t2)));
  }
}


/* Returns an instantiation of this formula. */
const Formula& Inequality::instantiation(const SubstitutionList& subst,
					 const Problem& problem) const {
  return substitution(subst);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Inequality::substitution(const SubstitutionList& subst) const {
  const Term& t1 = term1.substitution(subst);
  const Term& t2 = term2.substitution(subst);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 != t2) ? TRUE : FALSE;
  } else {
    return ((&t1 == &term1 && &t2 == &term2)
	    ? *this : *(new Inequality(t1, t2)));
  }
}


/* Returns this formula with static predicates assumed true. */
const Formula& Inequality::strip_static(const Domain& domain) const {
  return *this;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Inequality::strip_equality() const {
  return TRUE;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Inequality::equivalent(const Formula& f) const {
  const Inequality* neq = dynamic_cast<const Inequality*>(&f);
  return (neq != NULL
	  && term1.equivalent(neq->term1) && term2.equivalent(neq->term2));
}


/* Checks if this object equals the given object. */
bool Inequality::equals(const EqualityComparable& o) const {
  const Inequality* neq = dynamic_cast<const Inequality*>(&o);
  return neq != NULL && term1 == neq->term1 && term2 == neq->term2;
}


/* Prints this object on the given stream. */
void Inequality::print(ostream& os) const {
  os << "(not (= " << term1 << ' ' << term2 << "))";
}


/* Returns the negation of this formula. */
const Formula& Inequality::negation() const {
  return *(new Equality(term1, term2));
}


/* Returns an instantiation of this formula. */
const Conjunction& Conjunction::instantiation(size_t id) const {
  return *(new Conjunction(conjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const Bindings& bindings) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->instantiation(bindings));
  }
  return *c;
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    const Formula& ci = (*fi)->instantiation(subst, problem);
    const Conjunction* conj = dynamic_cast<const Conjunction*>(c);
    if (conj != NULL
	&& find_if(conj->conjuncts.begin(), conj->conjuncts.end(),
		   bind1st(equal_to<const EqualityComparable*>(), &!ci))
	!= conj->conjuncts.end()) {
      return FALSE;
    }
    c = &(*c && ci);
  }
  return *c;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Conjunction::substitution(const SubstitutionList& subst) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->substitution(subst));
  }
  return *c;
}


/* Returns this formula with static predicates assumed true. */
const Formula& Conjunction::strip_static(const Domain& domain) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->strip_static(domain));
  }
  return *c;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Conjunction::strip_equality() const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->strip_equality());
  }
  return *c;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Conjunction::equivalent(const Formula& f) const {
  const Conjunction* conjunction = dynamic_cast<const Conjunction*>(&f);
  return conjunction != NULL && conjuncts.equivalent(conjunction->conjuncts);
}


/* Checks if this object equals the given object. */
bool Conjunction::equals(const EqualityComparable& o) const {
  const Conjunction* conj = dynamic_cast<const Conjunction*>(&o);
  return (conj != NULL && conjuncts.size() == conj->conjuncts.size()
	  && equal(conjuncts.begin(), conjuncts.end(), conj->conjuncts.begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Prints this object on the given stream. */
void Conjunction::print(ostream& os) const {
  os << "(and";
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Conjunction::negation() const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    d = &(*d || !**fi);
  }
  return *d;
}


/* Returns an instantiation of this formula. */
const Disjunction& Disjunction::instantiation(size_t id) const {
  return *(new Disjunction(disjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const Bindings& bindings) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->instantiation(bindings));
  }
  return *d;
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* d = &TRUE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->instantiation(subst, problem));
  }
  return *d;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Disjunction::substitution(const SubstitutionList& subst) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->substitution(subst));
  }
  return *d;
}


/* Returns this formula with static predicates assumed true. */
const Formula& Disjunction::strip_static(const Domain& domain) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->strip_static(domain));
  }
  return *d;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Disjunction::strip_equality() const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->strip_equality());
  }
  return *d;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool Disjunction::equivalent(const Formula& f) const {
  const Disjunction* disjunction = dynamic_cast<const Disjunction*>(&f);
  return disjunction != NULL && disjuncts.equivalent(disjunction->disjuncts);
}


/* Checks if this object equals the given object. */
bool Disjunction::equals(const EqualityComparable& o) const {
  const Disjunction* disj = dynamic_cast<const Disjunction*>(&o);
  return (disj != NULL && disjuncts.size() == disj->disjuncts.size()
	  && equal(disjuncts.begin(), disjuncts.end(), disj->disjuncts.begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Prints this object on the given stream. */
void Disjunction::print(ostream& os) const {
  os << "(or";
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Disjunction::negation() const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    c = &(*c && !**fi);
  }
  return *c;
}


/* Returns an instantiation of this formula. */
const ExistsFormula& ExistsFormula::instantiation(size_t id) const {
  return *(new ExistsFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const Bindings& bindings) const {
  const Formula& b = body.instantiation(bindings);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ExistsFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula with static predicates assumed true. */
const Formula& ExistsFormula::strip_static(const Domain& domain) const {
  const Formula& b = body.strip_static(domain);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& ExistsFormula::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool ExistsFormula::equivalent(const Formula& f) const {
  const ExistsFormula* exists = dynamic_cast<const ExistsFormula*>(&f);
  return (exists != NULL && parameters.size() == exists->parameters.size()
	  && body.equivalent(exists->body));
}


/* Checks if this object equals the given object. */
bool ExistsFormula::equals(const EqualityComparable& o) const {
  const ExistsFormula* exists = dynamic_cast<const ExistsFormula*>(&o);
  return (exists != NULL && parameters.size() == exists->parameters.size()
	  && equal(parameters.begin(), parameters.end(),
		   exists->parameters.begin(),
		   equal_to<const EqualityComparable*>())
	  && body == exists->body);
}


/* Prints this object on the given stream. */
void ExistsFormula::print(ostream& os) const {
  os << "(exists (";
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    if (vi != parameters.begin()) {
      os << ' ';
    }
    os << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ") " << body << ")";
}


/* Returns the negation of this formula. */
const Formula& ExistsFormula::negation() const {
  return *(new ForallFormula(parameters, !body));
}


/* Returns an instantiation of this formula. */
const ForallFormula& ForallFormula::instantiation(size_t id) const {
  return *(new ForallFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const Bindings& bindings) const {
  const Formula& b = body.instantiation(bindings);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ForallFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula with static predicates assumed true. */
const Formula& ForallFormula::strip_static(const Domain& domain) const {
  const Formula& b = body.strip_static(domain);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& ForallFormula::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas is equivalent if they only differ in the choice of
   variable names. */
bool ForallFormula::equivalent(const Formula& f) const {
  const ForallFormula* forall = dynamic_cast<const ForallFormula*>(&f);
  return (forall != NULL && parameters.size() == forall->parameters.size()
	  && body.equivalent(forall->body));
}


/* Checks if this object equals the given object. */
bool ForallFormula::equals(const EqualityComparable& o) const {
  const ForallFormula* forall = dynamic_cast<const ForallFormula*>(&o);
  return (forall != NULL && parameters.size() == forall->parameters.size()
	  && equal(parameters.begin(), parameters.end(),
		   forall->parameters.begin(),
		   equal_to<const EqualityComparable*>())
	  && body == forall->body);
}


/* Prints this object on the given stream. */
void ForallFormula::print(ostream& os) const {
  os << "(forall (";
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    if (vi != parameters.begin()) {
      os << ' ';
    }
    os << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ") " << body << ")";
}


/* Returns the negation of this formula. */
const Formula& ForallFormula::negation() const {
  return *(new ExistsFormula(parameters, !body));
}
