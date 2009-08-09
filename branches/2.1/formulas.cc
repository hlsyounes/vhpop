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
 * $Id: formulas.cc,v 4.4 2002-09-20 16:46:32 lorens Exp $
 */
#include <typeinfo>
#include <stack>
#include "formulas.h"
#include "bindings.h"
#include "problems.h"
#include "domains.h"
#include "types.h"


/*
 * A substitutes binary predicate.
 */
struct Substitutes
  : public binary_function<Substitution, const Variable*, bool> {
  /* Constructs a substitutes binary predicate. */
  Substitutes(size_t step_id)
    : step_id_(step_id) {}

  /* Checks if the given substitution involves the given variable. */
  bool operator()(const Substitution& s, const Variable* v) const {
    return s.var() == *v && s.var_id() == step_id_;
  }

private:
  /* Step id to use for variables. */
  size_t step_id_;
};


/* ====================================================================== */
/* Substitution */

/* Constructs a substitution. */
Substitution::Substitution(const Variable& var, const Term& term)
  : var_(&var), var_id_(0), term_(&term), term_id_(0) {}


/* Constructs a substitution with assigned step ids. */
Substitution::Substitution(const Variable& var, size_t var_id,
			   const Term& term, size_t term_id)
  : var_(&var), var_id_(var_id), term_(&term), term_id_(term_id) {}


/* ====================================================================== */
/* Term */

/* Constructs an abstract term with the given name. */
Term::Term(const string& name, const Type& type)
  : name_(name), type_(&type) {}


/* Deletes this term. */
Term::~Term() {
  const UnionType* ut = dynamic_cast<const UnionType*>(type_);
  if (ut != NULL) {
    delete ut;
  }
}


/* Checks if this object is less than the given object. */
bool Term::less(const LessThanComparable& o) const {
  const Term& t = dynamic_cast<const Term&>(o);
  return name() < t.name();
}


/* Checks if this object equals the given object. */
bool Term::equals(const EqualityComparable& o) const {
  return this == &o;
}


/* Returns the hash value of this object. */
size_t Term::hash_value() const {
  return size_t(this);
}


/* Prints this term on the given stream with the given bindings. */
void Term::print(ostream& os, size_t step_id, const Bindings& bindings) const {
  os << bindings.binding(*this, step_id);
}


/* Prints this object on the given stream. */
void Term::print(ostream& os) const {
  os << name();
}


/* ====================================================================== */
/* Name */

/* Constructs a name. */
Name::Name(const string& name, const Type& type)
  : Term(name, type) {}


/* Returns this term subject to the given substitutions. */
const Name& Name::substitution(const SubstitutionList& subst,
			       size_t step_id) const {
  return *this;
}


/* ====================================================================== */
/* Variable */

/* Constructs a variable with the given name and type. */
Variable::Variable(const string& name, const Type& type)
  : Term(name, type) {}


/* Returns this term subject to the given substitutions. */
const Term& Variable::substitution(const SubstitutionList& subst,
				   size_t step_id) const {
  SubstListIter si = find_if(subst.begin(), subst.end(),
			     bind2nd(Substitutes(step_id), this));
  return (si != subst.end()) ? (*si).term() : *this;
}


/* ====================================================================== */
/* TermList */

/* Returns this term list subject to the given substitutions. */
const TermList& TermList::substitution(const SubstitutionList& subst,
				       size_t step_id) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->substitution(subst, step_id));
  }
  return terms;
}


/* ====================================================================== */
/* NameList */

/* An empty name list. */
const NameList NameList::EMPTY = NameList();


/* ====================================================================== */
/* VariableList */

/* An empty variable list. */
const VariableList VariableList::EMPTY = VariableList();


/* ====================================================================== */
/* Formula */

/* The true formula. */
const Formula& Formula::TRUE = Constant::TRUE_;
/* The false formula. */
const Formula& Formula::FALSE = Constant::FALSE_;


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
      copy(c1->conjuncts().begin(), c1->conjuncts().end(),
	   back_inserter(conjuncts));
    } else {
      conjuncts.push_back(&f1);
    }
    const Conjunction* c2 = dynamic_cast<const Conjunction*>(&f2);
    if (c2 != NULL) {
      copy(c2->conjuncts().begin(), c2->conjuncts().end(),
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
      copy(d1->disjuncts().begin(), d1->disjuncts().end(),
	   back_inserter(disjuncts));
    } else {
      disjuncts.push_back(&f1);
    }
    const Disjunction* d2 = dynamic_cast<const Disjunction*>(&f2);
    if (d2 != NULL) {
      copy(d2->disjuncts().begin(), d2->disjuncts().end(),
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
  : vector<const Formula*>(1, formula) {}


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

/* Constant representing true. */
const Constant Constant::TRUE_ = Constant(true);
/* Constant representing false. */
const Constant Constant::FALSE_ = Constant(false);


/* Constructs a constant formula. */
Constant::Constant(bool value)
  : value_(value) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Constant::separate(const Literal& literal) const {
  return TRUE;
}


/* Returns an instantiation of this formula. */
const Constant& Constant::instantiation(const SubstitutionList& subst,
					const Problem& problem) const {
  return *this;
}


/* Returns this formula subject to the given substitutions. */
const Constant& Constant::substitution(const SubstitutionList& subst,
				       size_t step_id) const {
  return *this;
}


/* Returns this formula with static literals assumed true. */
const Constant& Constant::strip_static(const Domain& domain) const {
  return *this;
}


/* Prints this formula on the given stream with the given bindings. */
void Constant::print(ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  print(os);
}


/* Prints this object on the given stream. */
void Constant::print(ostream& os) const {
  os << (value_ ? "TRUE" : "FALSE");
}


/* Returns a negation of this formula. */
const Formula& Constant::negation() const {
  return value_ ? FALSE : TRUE;
}


/* ====================================================================== */
/* Literal */

/* Constructs a literal. */
Literal::Literal(FormulaTime when)
  : when_(when) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Literal::separate(const Literal& literal) const {
  if (when() == literal.when()
      || when() == OVER_ALL || literal.when() == OVER_ALL) {
    SubstitutionList mgu;
    if (Bindings::unifiable(mgu, *this, 0, literal, 0)) {
      const Formula* sep = &FALSE;
      for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	const Substitution& subst = *si;
	if (subst.var() != subst.term()) {
	  sep = &(*sep || *(new Inequality(subst.var(), subst.term())));
	}
      }
      return *sep;
    }
  }
  return TRUE;
}


/* Returns this formula with static literals assumed true. */
const Formula& Literal::strip_static(const Domain& domain) const {
  if (domain.static_predicate(predicate())
      || domain.find_type(predicate().name()) != NULL) {
    return TRUE;
  } else {
    return *this;
  }
}


/* ====================================================================== */
/* Atom */

/* Constructs an atomic formula. */
Atom::Atom(const Predicate& predicate, const TermList& terms, FormulaTime when)
  : Literal(when), predicate_(&predicate), terms_(&terms) {}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const SubstitutionList& subst,
				   const Problem& problem) const {
  const Atom& f = substitution(subst, 0);
  if (problem.domain().static_predicate(predicate())) {
    const AtomList& adds = problem.init().add_list();
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      if (f == **gi) {
	return TRUE;
      } else if (Bindings::unifiable(f, 0, **gi, 0)) {
	return f;
      }
    }
    return FALSE;
  } else {
    const Type* type = problem.domain().find_type(predicate().name());
    if (type != NULL) {
      return f.terms()[0]->type().subtype(*type) ? TRUE : FALSE;
    } else {
      return f;
    }
  }
}


/* Returns this formula subject to the given substitutions. */
const Atom& Atom::substitution(const SubstitutionList& subst,
			       size_t step_id) const {
  return *(new Atom(predicate(), terms().substitution(subst, step_id),
		    when()));
}


/* Checks if this object equals the given object. */
bool Atom::equals(const Literal& o) const {
  const Atom* atom = dynamic_cast<const Atom*>(&o);
  return (atom != NULL && predicate() == atom->predicate()
	  && terms().size() == atom->terms().size()
	  && equal(terms().begin(), terms().end(), atom->terms().begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Returns the hash value of this object. */
size_t Atom::hash_value() const {
  hash<Hashable> h;
  size_t val = size_t(&predicate());
  for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
    val = 5*val + h(**ti);
  }
  return val;
}


/* Prints this formula on the given stream with the given bindings. */
void Atom::print(ostream& os, size_t step_id, const Bindings& bindings) const {
  os << '(' << predicate().name();
  for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
    os << ' ';
    (*ti)->print(os, step_id, bindings);
  }
  os << ')';
}


/* Prints this object on the given stream. */
void Atom::print(ostream& os) const {
  os << '(' << predicate().name();
  copy(terms().begin(), terms().end(), pre_ostream_iterator<Term>(os));
  os << ')';
}


/* Returns the negation of this formula. */
const Literal& Atom::negation() const {
  return *(new Negation(*this));
}


/* ====================================================================== */
/* Negation */

/* Constructs a negated atom. */
Negation::Negation(const Atom& atom)
  : Literal(atom.when()), atom_(&atom) {}


/* Returns an instantiation of this formula. */
const Formula& Negation::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return !atom().instantiation(subst, problem);
}


/* Returns this formula subject to the given substitutions. */
const Negation& Negation::substitution(const SubstitutionList& subst,
				       size_t step_id) const {
  return *(new Negation(atom().substitution(subst, step_id)));
}


/* Checks if this object equals the given object. */
bool Negation::equals(const Literal& o) const {
  const Negation* negation = dynamic_cast<const Negation*>(&o);
  return negation != NULL && atom() == negation->atom();
}


/* Returns the hash value of this object. */
size_t Negation::hash_value() const {
  return 5*hash<Literal>()(atom());
}


/* Prints this formula on the given stream with the given bindings. */
void Negation::print(ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  os << "(not ";
  atom().print(os, step_id, bindings);
  os << ")";
}


/* Prints this object on the given stream. */
void Negation::print(ostream& os) const {
  os << "(not " << atom() << ")";
}


/* Returns the negation of this formula. */
const Literal& Negation::negation() const {
  return atom();
}


/* ====================================================================== */
/* BindingLiteral */

/* Constructs a binding literal. */
BindingLiteral::BindingLiteral(const Term& term1, size_t id1,
			       const Term& term2, size_t id2)
  : term1_(&term1), id1_(id1), term2_(&term2), id2_(id2) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& BindingLiteral::separate(const Literal& literal) const {
  return TRUE;
}


/* ====================================================================== */
/* Equality */

/* Constructs an equality. */
Equality::Equality(const Term& term1, const Term& term2)
  : BindingLiteral(term1, 0, term2, 0) {}


/* Constructs an equality with assigned step ids. */
Equality::Equality(const Term& term1, size_t id1,
		   const Term& term2, size_t id2)
  : BindingLiteral(term1, id1, term2, id2) {}


/* Returns an instantiation of this formula. */
const Formula& Equality::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return substitution(subst, 0);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Equality::substitution(const SubstitutionList& subst,
				      size_t step_id) const {
  const Term& t1 = term1().substitution(subst, step_id);
  const Term& t2 = term2().substitution(subst, step_id);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 == t2) ? TRUE : FALSE;
  } else {
    return ((&t1 == &term1() && &t2 == &term2())
	    ? *this : *(new Equality(t1, step_id1(0), t2, step_id2(0))));
  }
}


/* Returns this formula with static literals assumed true. */
const Formula& Equality::strip_static(const Domain& domain) const {
  return *this;
}


/* Prints this formula on the given stream with the given bindings. */
void Equality::print(ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  os << "(= ";
  term1().print(os, step_id, bindings);
  os << ' ';
  term2().print(os, step_id, bindings);
  os << ")";
}


/* Prints this object on the given stream. */
void Equality::print(ostream& os) const {
  os << "(= " << term1() << ' ' << term2() << ")";
}


/* Returns the negation of this formula. */
const BindingLiteral& Equality::negation() const {
  return *(new Inequality(term1(), step_id1(0), term2(), step_id2(0)));
}


/* ====================================================================== */
/* Inequality */

/* Constructs an inequality. */
Inequality::Inequality(const Term& term1, const Term& term2)
  : BindingLiteral(term1, 0, term2, 0) {}


/* Constructs an inequality with assigned step ids. */
Inequality::Inequality(const Term& term1, size_t id1,
		       const Term& term2, size_t id2)
  : BindingLiteral(term1, id1, term2, id2) {}


/* Returns an instantiation of this formula. */
const Formula& Inequality::instantiation(const SubstitutionList& subst,
					 const Problem& problem) const {
  return substitution(subst, 0);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Inequality::substitution(const SubstitutionList& subst,
					size_t step_id) const {
  const Term& t1 = term1().substitution(subst, step_id);
  const Term& t2 = term2().substitution(subst, step_id);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 != t2) ? TRUE : FALSE;
  } else {
    return ((&t1 == &term1() && &t2 == &term2())
	    ? *this : *(new Inequality(t1, step_id1(0), t2, step_id2(0))));
  }
}


/* Returns this formula with static literals assumed true. */
const Formula& Inequality::strip_static(const Domain& domain) const {
  return *this;
}


/* Prints this formula on the given stream with the given bindings. */
void Inequality::print(ostream& os, size_t step_id,
		       const Bindings& bindings) const {
  os << "(not (= ";
  term1().print(os, step_id, bindings);
  os << ' ';
  term2().print(os, step_id, bindings);
  os << "))";
}


/* Prints this object on the given stream. */
void Inequality::print(ostream& os) const {
  os << "(not (= " << term1() << ' ' << term2() << "))";
}


/* Returns the negation of this formula. */
const BindingLiteral& Inequality::negation() const {
  return *(new Equality(term1(), step_id1(0), term2(), step_id2(0)));
}


/* ====================================================================== */
/* Conjunction */

/* Constructs a conjunction. */
Conjunction::Conjunction(const FormulaList& conjuncts)
  : conjuncts_(&conjuncts) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Conjunction::separate(const Literal& literal) const {
  const Formula* sep = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    sep = &(*sep && (*fi)->separate(literal));
  }
  return *sep;
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->instantiation(subst, problem));
  }
  return *c;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Conjunction::substitution(const SubstitutionList& subst,
					 size_t step_id) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->substitution(subst, step_id));
  }
  return *c;
}


/* Returns this formula with static literals assumed true. */
const Formula& Conjunction::strip_static(const Domain& domain) const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end() && !c->contradiction(); fi++) {
    c = &(*c && (*fi)->strip_static(domain));
  }
  return *c;
}


/* Prints this formula on the given stream with the given bindings. */
void Conjunction::print(ostream& os, size_t step_id,
			const Bindings& bindings) const {
  os << "(and";
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, step_id, bindings);
  }
  os << ")";
}


/* Prints this object on the given stream. */
void Conjunction::print(ostream& os) const {
  os << "(and";
  copy(conjuncts().begin(), conjuncts().end(),
       pre_ostream_iterator<Formula>(os));
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Conjunction::negation() const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end() && !d->tautology(); fi++) {
    d = &(*d || !**fi);
  }
  return *d;
}


/* ====================================================================== */
/* Disjunction */

/* Constructs a disjunction. */
Disjunction::Disjunction(const FormulaList& disjuncts)
  : disjuncts_(&disjuncts) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Disjunction::separate(const Literal& literal) const {
  /* We are being conservative here.  It can be hard to find a
     separator in this case. */
  return TRUE;
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->instantiation(subst, problem));
  }
  return *d;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Disjunction::substitution(const SubstitutionList& subst,
					 size_t step_id) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end() && d->tautology(); fi++) {
    d = &(*d || (*fi)->substitution(subst, step_id));
  }
  return *d;
}


/* Returns this formula with static literals assumed true. */
const Formula& Disjunction::strip_static(const Domain& domain) const {
  const Formula* d = &FALSE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end() && !d->tautology(); fi++) {
    d = &(*d || (*fi)->strip_static(domain));
  }
  return *d;
}


/* Prints this formula on the given stream with the given bindings. */
void Disjunction::print(ostream& os, size_t step_id,
			const Bindings& bindings) const {
  os << "(or";
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, step_id, bindings);
  }
  os << ")";
}


/* Prints this object on the given stream. */
void Disjunction::print(ostream& os) const {
  os << "(or";
  copy(disjuncts().begin(), disjuncts().end(),
       pre_ostream_iterator<Formula>(os));
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Disjunction::negation() const {
  const Formula* c = &TRUE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end() && !c->contradiction(); fi++) {
    c = &(*c && !**fi);
  }
  return *c;
}


/* ====================================================================== */
/* QuantifiedFormula */

/* Constructs a quantified formula. */
QuantifiedFormula::QuantifiedFormula(const VariableList& parameters,
				     const Formula& body)
  : parameters_(&parameters), body_(&body) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& QuantifiedFormula::separate(const Literal& literal) const {
  /* We are being conservative here.  It can be hard to find a
     separator in this case. */
  return TRUE;
}


/* ====================================================================== */
/* ExistsFormula */

/* Constructs an existentially quantified formula. */
ExistsFormula::ExistsFormula(const VariableList& parameters,
			     const Formula& body)
  : QuantifiedFormula(parameters, body) {}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body().instantiation(subst, problem);
  size_t n = parameters().size();
  vector<NameList*> arguments;
  vector<NameListIter> next_arg;
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*vi)->type());
    if (arguments.back()->empty()) {
      return FALSE;
    }
    next_arg.push_back(arguments.back()->begin());
  }
  const Formula* result = &FALSE;
  stack<const Formula*> disjuncts;
  disjuncts.push(&b);
  for (size_t i = 0; i < n; ) {
    SubstitutionList pargs;
    pargs.push_back(Substitution(*parameters()[i], **next_arg[i]));
    const Formula& disjunct = disjuncts.top()->instantiation(pargs, problem);
    disjuncts.push(&disjunct);
    if (i + 1 == n) {
      result = &(*result || disjunct);
      if (result->tautology()) {
	break;
      }
      for (int j = i; j >= 0; j--) {
	disjuncts.pop();
	next_arg[j]++;
	if (next_arg[j] == arguments[j]->end()) {
	  if (j == 0) {
	    i = n;
	    break;
	  } else {
	    next_arg[j] = arguments[j]->begin();
	  }
	} else {
	  i = j;
	  break;
	}
      }
    } else {
      i++;
    }
  }
  return *result;
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ExistsFormula::substitution(const SubstitutionList& subst,
			    size_t step_id) const {
  const Formula& b = body().substitution(subst, step_id);
  return (b.constant()
	  ? b : (const Formula&) *(new ExistsFormula(parameters(), b)));
}


/* Returns this formula with static literals assumed true. */
const Formula& ExistsFormula::strip_static(const Domain& domain) const {
  const Formula& b = body().strip_static(domain);
  return (b.constant()
	  ? b : (const Formula&) *(new ExistsFormula(parameters(), b)));
}


/* Prints this formula on the given stream with the given bindings. */
void ExistsFormula::print(ostream& os, size_t step_id,
			  const Bindings& bindings) const {
  os << "(exists (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    (*vi)->print(os, step_id, bindings);
    os << " - " << (*vi)->type();
  }
  os << ") ";
  body().print(os, step_id, bindings);
  os << ")";
}


/* Prints this object on the given stream. */
void ExistsFormula::print(ostream& os) const {
  os << "(exists (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << **vi << " - " << (*vi)->type();
  }
  os << ") " << body() << ")";
}


/* Returns the negation of this formula. */
const QuantifiedFormula& ExistsFormula::negation() const {
  return *(new ForallFormula(parameters(), !body()));
}


/* ====================================================================== */
/* ForallFormula */

/* Constructs a universally quantified formula. */
ForallFormula::ForallFormula(const VariableList& parameters,
			     const Formula& body)
  : QuantifiedFormula(parameters, body) {}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body().instantiation(subst, problem);
  size_t n = parameters().size();
  vector<NameList*> arguments;
  vector<NameListIter> next_arg;
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*vi)->type());
    if (arguments.back()->empty()) {
      return FALSE;
    }
    next_arg.push_back(arguments.back()->begin());
  }
  const Formula* result = &TRUE;
  stack<const Formula*> conjuncts;
  conjuncts.push(&b);
  for (size_t i = 0; i < n; ) {
    SubstitutionList pargs;
    pargs.push_back(Substitution(*parameters()[i], **next_arg[i]));
    const Formula& conjunct = conjuncts.top()->instantiation(pargs, problem);
    conjuncts.push(&conjunct);
    if (i + 1 == n) {
      result = &(*result && conjunct);
      if (result->contradiction()) {
	break;
      }
      for (int j = i; j >= 0; j--) {
	conjuncts.pop();
	next_arg[j]++;
	if (next_arg[j] == arguments[j]->end()) {
	  if (j == 0) {
	    i = n;
	    break;
	  } else {
	    next_arg[j] = arguments[j]->begin();
	  }
	} else {
	  i = j;
	  break;
	}
      }
    } else {
      i++;
    }
  }
  return *result;
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ForallFormula::substitution(const SubstitutionList& subst,
			    size_t step_id) const {
  const Formula& b = body().substitution(subst, step_id);
  return (b.constant()
	  ? b : (const Formula&) *(new ForallFormula(parameters(), b)));
}


/* Returns this formula with static literals assumed true. */
const Formula& ForallFormula::strip_static(const Domain& domain) const {
  const Formula& b = body().strip_static(domain);
  return (b.constant()
	  ? b : (const Formula&) *(new ForallFormula(parameters(), b)));
}


/* Prints this formula on the given stream with the given bindings. */
void ForallFormula::print(ostream& os, size_t step_id,
			  const Bindings& bindings) const {
  os << "(forall (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    (*vi)->print(os, step_id, bindings);
    os << " - " << (*vi)->type();
  }
  os << ") ";
  body().print(os, step_id, bindings);
  os << ")";
}


/* Prints this object on the given stream. */
void ForallFormula::print(ostream& os) const {
  os << "(forall (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << **vi << " - " << (*vi)->type();
  }
  os << ") " << body() << ")";
}


/* Returns the negation of this formula. */
const QuantifiedFormula& ForallFormula::negation() const {
  return *(new ExistsFormula(parameters(), !body()));
}


/* ====================================================================== */
/* AtomList */

/* An empty atom list. */
const AtomList AtomList::EMPTY = AtomList();


/* Constructs an empty atom list. */
AtomList::AtomList() {}


/* Constructs an atom list with a single atom. */
AtomList::AtomList(const Atom* atom)
  : vector<const Atom*>(1, atom) {}


/* Returns this atom list subject to the given substitutions. */
const AtomList& AtomList::substitution(const SubstitutionList& subst,
				       size_t step_id) const {
  if (empty()) {
    return EMPTY;
  } else {
    AtomList& atoms = *(new AtomList());
    for (const_iterator i = begin(); i != end(); i++) {
      atoms.push_back(&(*i)->substitution(subst, step_id));
    }
    return atoms;
  }
}


/* ====================================================================== */
/* NegationList */

/* An empty negation list. */
const NegationList NegationList::EMPTY = NegationList();


/* Constructs an empty negation list. */
NegationList::NegationList() {}


/* Constructs a negation list with a single negated atom. */
NegationList::NegationList(const Atom* atom)
  : vector<const Negation*>(1, new Negation(*atom)) {}


/* Returns this negation list subject to the given substitutions. */
const NegationList&
NegationList::substitution(const SubstitutionList& subst,
			   size_t step_id) const {
  if (empty()) {
    return EMPTY;
  } else {
    NegationList& negations = *(new NegationList());
    for (const_iterator i = begin(); i != end(); i++) {
      negations.push_back(&(*i)->substitution(subst, step_id));
    }
    return negations;
  }
}
