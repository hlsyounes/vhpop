/*
 * $Id: formulas.cc,v 1.29 2001-12-26 18:51:47 lorens Exp $
 */
#include <typeinfo>
#include "formulas.h"
#include "types.h"
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


/*
 * An equivalent terms binary predicate.
 */
struct EquivalentTerms
  : public binary_function<const Term*, const Term*, bool> {
  /* Checks if the two terms are equivalent. */
  bool operator()(const Term* t1, const Term* t2) const {
    return t1->equivalent(*t2);
  }
};


/*
 * An equivalent formulas binary predicate.
 */
struct EquivalentFormulas
  : public binary_function<const Formula*, const Formula*, bool> {
  /* Checks if the two formulas are equivalent. */
  bool operator()(const Formula* f1, const Formula* f2) const {
    return f1->equivalent(*f2);
  }
};


/* ====================================================================== */
/* Substitution */

/* Constructs a substitution. */
Substitution::Substitution(const Variable& var, const Term& term)
  : var(var), term(term) {}


/* Prints this object on the given stream. */
void Substitution::print(ostream& os) const {
  os << '[' << var << '/' << term << ']';
}


/* ====================================================================== */
/* Term */

/* Constructs an abstract term with the given name. */
Term::Term(const string& name, const Type& type)
  : name(name), type(type) {}


/* Returns an instantiation of this term. */
const Term& Term::instantiation(const Bindings& bindings) const {
  return bindings.binding(*this);
}


/* Checks if this object is less than the given object. */
bool Term::less(const LessThanComparable& o) const {
  if (typeid(o) == typeid(StepVar)) {
    return true;
  } else {
    const Term& t = dynamic_cast<const Term&>(o);
    return name < t.name;
  }
}


/* Checks if this object equals the given object. */
bool Term::equals(const EqualityComparable& o) const {
  const Term* t = dynamic_cast<const Term*>(&o);
  return t != NULL && typeid(o) == typeid(*this) && name == t->name;
}


/* Returns the hash value of this object. */
size_t Term::hash_value() const {
  return hash<string>()(name);
}


/* Prints this object on the given stream. */
void Term::print(ostream& os) const {
  os << name;
}


/* ====================================================================== */
/* Name */

/* Constructs a name. */
Name::Name(const string& name, const Type& type)
  : Term(name, type) {}


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


/* ====================================================================== */
/* Variable */

/* Constructs a variable with the given name. */
Variable::Variable(const string& name)
  : Term(name, SimpleType::OBJECT) {}


/* Constructs a variable with the given name and type. */
Variable::Variable(const string& name, const Type& type)
  : Term(name, type) {}


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


/* ====================================================================== */
/* StepVar */

/* Constructs an instantiated variable. */
StepVar::StepVar(const Variable& var, size_t id)
  : Variable(var), id(id) {}


/* Checks if this object is less than the given object. */
bool StepVar::less(const LessThanComparable& o) const {
  if (typeid(o) != typeid(StepVar)) {
    return false;
  } else {
    const StepVar& vt = dynamic_cast<const StepVar&>(o);
    return id < vt.id || (id == vt.id && name < vt.name);
  }
}


/* Checks if this object equals the given object. */
bool StepVar::equals(const EqualityComparable& o) const {
  const StepVar* vt = dynamic_cast<const StepVar*>(&o);
  return vt != NULL && id == vt->id && name == vt->name;
}


/* Prints this object on the given stream. */
void StepVar::print(ostream& os) const {
  os << name << '(' << id << ')';
}


/* ====================================================================== */
/* TermList */

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


/* ====================================================================== */
/* VariableList */

/* An empty variable list. */
const VariableList& VariableList::EMPTY = *(new VariableList());


/* Returns an instantiation of this variable list. */
const VariableList& VariableList::instantiation(size_t id) const {
  VariableList& variables = *(new VariableList());
  for (const_iterator vi = begin(); vi != end(); vi++) {
    variables.push_back(&(*vi)->instantiation(id));
  }
  return variables;
}


/* ====================================================================== */
/* Formula */

/* The true formula. */
const Formula& Formula::TRUE = *(new Constant(true));
/* The false formula. */
const Formula& Formula::FALSE = *(new Constant(false));


/* Checks if this formula is a tautology. */
bool Formula::tautology() const {
  return this == &TRUE;
}


/* Checks if this formula is a contradiction. */
bool Formula::contradiction() const {
  return this == &FALSE;
}


/* Checks if this formula is either a tautology or contradiction. */
bool Formula::constant() const {
  return tautology() || contradiction();
}


/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2) {
  if (f1.contradiction() || f2.contradiction()) {
    return Formula::FALSE;
  } else if (f1.tautology()) {
    return f2;
  } else if (f2.tautology()) {
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
  if (f1.tautology() || f2.tautology()) {
    return Formula::TRUE;
  } else if (f1.contradiction()) {
    return f2;
  } else if (f2.contradiction()) {
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


/* ====================================================================== */
/* FormulaList */

/* Constructs an empty formula list. */
FormulaList::FormulaList() {}


/* Constructs a formula list with a single formula. */
FormulaList::FormulaList(const Formula* formula)
  : Vector<const Formula*>(1, formula) {}


/* Returns an instantiation of this formula list. */
const FormulaList& FormulaList::instantiation(size_t id) const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&(*i)->instantiation(id));
  }
  return formulas;
}


/* Returns the negation of this formula list. */
const FormulaList& FormulaList::negation() const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&!**i);
  }
  return formulas;
}


/* ====================================================================== */
/* Constant */

/* Constructs a constant formula. */
Constant::Constant(bool value)
  : value(value) {}


/* Returns an instantiation of this formula. */
const Constant& Constant::instantiation(size_t id) const {
  return *this;
}


/* Returns an instantiation of this formula. */
const Constant& Constant::instantiation(const Bindings& bindings) const {
  return *this;
}


/* Returns an instantiation of this formula. */
const Constant& Constant::instantiation(const SubstitutionList& subst,
					const Problem& problem) const {
  return *this;
}


/* Returns this formula subject to the given substitutions. */
const Constant& Constant::substitution(const SubstitutionList& subst) const {
  return *this;
}


/* Returns this formula with static predicates assumed true. */
const Constant& Constant::strip_static(const Domain& domain) const {
  return *this;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Constant& Constant::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Constant::equivalent(const Formula& f) const {
  return &f == this;
}


/* Prints this object on the given stream. */
void Constant::print(ostream& os) const {
  os << (value ? "TRUE" : "FALSE");
}


/* Returns a negation of this formula. */
const Formula& Constant::negation() const {
  return value ? FALSE : TRUE;
}


/* ====================================================================== */
/* Atom */

/* Constructs an atomic formula. */
Atom::Atom(const string& predicate, const TermList& terms)
  : predicate_(predicate), terms_(terms) {}


/* Returns the predicate of this literal. */
const string& Atom::predicate() const {
  return predicate_;
}


/* Returns the terms of this literal. */
const TermList& Atom::terms() const {
  return terms_;
}


/* Returns an instantiation of this formula. */
const Atom& Atom::instantiation(size_t id) const {
  return *(new Atom(predicate_, terms_.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const Bindings& bindings) const {
  return *(new Atom(predicate_, terms_.instantiation(bindings)));
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const SubstitutionList& subst,
				   const Problem& problem) const {
  const Atom& f = substitution(subst);
  if (problem.domain.static_predicate(predicate_)) {
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
    const Type* type = problem.domain.find_type(predicate_);
    if (type != NULL) {
      return f.terms_[0]->type.subtype(*type) ? TRUE : FALSE;
    } else {
      return f;
    }
  }
}


/* Returns this formula subject to the given substitutions. */
const Atom& Atom::substitution(const SubstitutionList& subst) const {
  return *(new Atom(predicate_, terms_.substitution(subst)));
}


/* Returns this formula with static predicates assumed true. */
const Formula& Atom::strip_static(const Domain& domain) const {
  if (domain.static_predicate(predicate_)
      || domain.find_type(predicate_) != NULL) {
    return TRUE;
  } else {
    return *this;
  }
}


/* Returns this formula with equalities/inequalities assumed true. */
const Atom& Atom::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Atom::equivalent(const Formula& f) const {
  const Atom* atom = dynamic_cast<const Atom*>(&f);
  return (atom != NULL && predicate_ == atom->predicate_
	  && terms_.size() == atom->terms_.size()
	  && equal(terms_.begin(), terms_.end(), atom->terms_.begin(),
		   EquivalentTerms()));
}


/* Checks if this object equals the given object. */
bool Atom::equals(const Literal& o) const {
  const Atom* atom = dynamic_cast<const Atom*>(&o);
  return (atom != NULL && predicate_ == atom->predicate_
	  && terms_.size() == atom->terms_.size()
	  && equal(terms_.begin(), terms_.end(), atom->terms_.begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Returns the hash value of this object. */
size_t Atom::hash_value() const {
  hash<Hashable> h;
  size_t val = hash<string>()(predicate_);
  for (TermListIter ti = terms_.begin(); ti != terms_.end(); ti++) {
    val = 5*val + h(**ti);
  }
  return val;
}


/* Prints this object on the given stream. */
void Atom::print(ostream& os) const {
  os << '(' << predicate_;
  for (TermListIter ti = terms_.begin(); ti != terms_.end(); ti++) {
    os << ' ' << **ti;
  }
  os << ')';
}


/* Returns the negation of this formula. */
const Formula& Atom::negation() const {
  return *(new Negation(*this));
}


/* ====================================================================== */
/* Negation */

/* Constructs a negated atom. */
Negation::Negation(const Atom& atom)
  : atom(atom) {}


/* Returns the predicate of this literal. */
const string& Negation::predicate() const {
  return atom.predicate();
}


/* Returns the terms of this literal. */
const TermList& Negation::terms() const {
  return atom.terms();
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
  if (domain.static_predicate(predicate())
      || domain.find_type(predicate()) != NULL) {
    return FALSE;
  } else {
    return *this;
  }
}


/* Returns this formula with equalities/inequalities assumed true. */
const Negation& Negation::strip_equality() const {
  return *this;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Negation::equivalent(const Formula& f) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  return negation != NULL && atom.equivalent(negation->atom);
}


/* Checks if this object equals the given object. */
bool Negation::equals(const Literal& o) const {
  const Negation* negation = dynamic_cast<const Negation*>(&o);
  return negation != NULL && atom == negation->atom;
}


/* Returns the hash value of this object. */
size_t Negation::hash_value() const {
  return 5*hash<Literal>()(atom);
}


/* Prints this object on the given stream. */
void Negation::print(ostream& os) const {
  os << "(not " << atom << ")";
}


/* Returns the negation of this formula. */
const Formula& Negation::negation() const {
  return atom;
}


/* ====================================================================== */
/* Equality */

/* Constructs an equality. */
Equality::Equality(const Term& term1, const Term& term2)
  : term1(term1), term2(term2) {}


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
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Equality::equivalent(const Formula& f) const {
  const Equality* eq = dynamic_cast<const Equality*>(&f);
  return (eq != NULL
	  && term1.equivalent(eq->term1) && term2.equivalent(eq->term2));
}


/* Prints this object on the given stream. */
void Equality::print(ostream& os) const {
  os << "(= " << term1 << ' ' << term2 << ")";
}


/* Returns the negation of this formula. */
const Formula& Equality::negation() const {
  return *(new Inequality(term1, term2));
}


/* ====================================================================== */
/* Inequality */

/* Constructs an inequality. */
Inequality::Inequality(const Term& term1, const Term& term2)
  : term1(term1), term2(term2) {}


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
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Inequality::equivalent(const Formula& f) const {
  const Inequality* neq = dynamic_cast<const Inequality*>(&f);
  return (neq != NULL
	  && term1.equivalent(neq->term1) && term2.equivalent(neq->term2));
}


/* Prints this object on the given stream. */
void Inequality::print(ostream& os) const {
  os << "(not (= " << term1 << ' ' << term2 << "))";
}


/* Returns the negation of this formula. */
const Formula& Inequality::negation() const {
  return *(new Equality(term1, term2));
}


/* ====================================================================== */
/* Conjunction */

/* Constructs a conjunction. */
Conjunction::Conjunction(const FormulaList& conjuncts)
  : conjuncts(conjuncts) {}


/* Returns an instantiation of this formula. */
const Conjunction& Conjunction::instantiation(size_t id) const {
  return *(new Conjunction(conjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const Bindings& bindings) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->instantiation(bindings));
  }
  return *c;
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !c->contradiction(); fi++) {
    const Formula& ci = (*fi)->instantiation(subst, problem);
    c = &(*c && ci);
  }
  return *c;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Conjunction::substitution(const SubstitutionList& subst) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->substitution(subst));
  }
  return *c;
}


/* Returns this formula with static predicates assumed true. */
const Formula& Conjunction::strip_static(const Domain& domain) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->strip_static(domain));
  }
  return *c;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Conjunction::strip_equality() const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->strip_equality());
  }
  return *c;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Conjunction::equivalent(const Formula& f) const {
  const Conjunction* conj = dynamic_cast<const Conjunction*>(&f);
  return (conj != NULL  && conjuncts.size() == conj->conjuncts.size()
	  && equal(conjuncts.begin(), conjuncts.end(), conj->conjuncts.begin(),
		   EquivalentFormulas()));
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
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !d->tautology(); fi++) {
    d = &(*d || !**fi);
  }
  return *d;
}


/* ====================================================================== */
/* Disjunction */

/* Constructs a disjunction. */
Disjunction::Disjunction(const FormulaList& disjuncts)
  : disjuncts(disjuncts) {}


/* Returns an instantiation of this formula. */
const Disjunction& Disjunction::instantiation(size_t id) const {
  return *(new Disjunction(disjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const Bindings& bindings) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->instantiation(bindings));
  }
  return *d;
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* d = &TRUE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->instantiation(subst, problem));
  }
  return *d;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Disjunction::substitution(const SubstitutionList& subst) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && d->tautology(); fi++) {
    d = &(*d || (*fi)->substitution(subst));
  }
  return *d;
}


/* Returns this formula with static predicates assumed true. */
const Formula& Disjunction::strip_static(const Domain& domain) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->strip_static(domain));
  }
  return *d;
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& Disjunction::strip_equality() const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->strip_equality());
  }
  return *d;
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool Disjunction::equivalent(const Formula& f) const {
  const Disjunction* disj = dynamic_cast<const Disjunction*>(&f);
  return (disj != NULL && disjuncts.size() == disj->disjuncts.size()
	  && equal(disjuncts.begin(), disjuncts.end(), disj->disjuncts.begin(),
		   EquivalentFormulas()));
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
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !c->contradiction(); fi++) {
    c = &(*c && !**fi);
  }
  return *c;
}


/* ====================================================================== */
/* QuantifiedFormula */

/* Constructs a quantified formula. */
QuantifiedFormula::QuantifiedFormula(const VariableList& parameters,
				     const Formula& body)
  : parameters(parameters), body(body) {}


/* ====================================================================== */
/* ExistsFormula */

/* Constructs an existentially quantified formula. */
ExistsFormula::ExistsFormula(const VariableList& parameters,
			     const Formula& body)
  : QuantifiedFormula(parameters, body) {}


/* Returns an instantiation of this formula. */
const ExistsFormula& ExistsFormula::instantiation(size_t id) const {
  return *(new ExistsFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const Bindings& bindings) const {
  const Formula& b = body.instantiation(bindings);
  return b.constant() ? b : *(new ExistsFormula(parameters, b));
}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return b.constant() ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ExistsFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return b.constant() ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula with static predicates assumed true. */
const Formula& ExistsFormula::strip_static(const Domain& domain) const {
  const Formula& b = body.strip_static(domain);
  return b.constant() ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& ExistsFormula::strip_equality() const {
  const Formula& b = body.strip_equality();
  return b.constant() ? b : *(new ExistsFormula(parameters, b));
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool ExistsFormula::equivalent(const Formula& f) const {
  const ExistsFormula* exists = dynamic_cast<const ExistsFormula*>(&f);
  return (exists != NULL && parameters.size() == exists->parameters.size()
	  && body.equivalent(exists->body));
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


/* ====================================================================== */
/* ForallFormula */

/* Constructs a universally quantified formula. */
ForallFormula::ForallFormula(const VariableList& parameters,
			     const Formula& body)
  : QuantifiedFormula(parameters, body) {}


/* Returns an instantiation of this formula. */
const ForallFormula& ForallFormula::instantiation(size_t id) const {
  return *(new ForallFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const Bindings& bindings) const {
  const Formula& b = body.instantiation(bindings);
  return b.constant() ? b : *(new ForallFormula(parameters, b));
}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return b.constant() ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ForallFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return b.constant() ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula with static predicates assumed true. */
const Formula& ForallFormula::strip_static(const Domain& domain) const {
  const Formula& b = body.strip_static(domain);
  return b.constant() ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula with equalities/inequalities assumed true. */
const Formula& ForallFormula::strip_equality() const {
  const Formula& b = body.strip_equality();
  return b.constant() ? b : *(new ForallFormula(parameters, b));
}


/* Checks if this formula is equivalent to the given formula.  Two
   formulas are equivalent if they only differ in the choice of
   variable names. */
bool ForallFormula::equivalent(const Formula& f) const {
  const ForallFormula* forall = dynamic_cast<const ForallFormula*>(&f);
  return (forall != NULL && parameters.size() == forall->parameters.size()
	  && body.equivalent(forall->body));
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


/* ====================================================================== */
/* AtomList */

/* Constructs an empty atom list. */
AtomList::AtomList() {}


/* Constructs an atom list with a single atom. */
AtomList::AtomList(const Atom* atom)
  : Vector<const Atom*>(1, atom) {}


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


/* ====================================================================== */
/* NegationList */

/* Constructs an empty negation list. */
NegationList::NegationList() {}


/* Constructs a negation list with a single negated atom. */
NegationList::NegationList(const Atom* atom)
  : Vector<const Negation*>(1, new Negation(*atom)) {}


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
