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
 * $Id: formulas.cc,v 4.10 2002-12-16 17:28:20 lorens Exp $
 */
#include "formulas.h"
#include "bindings.h"
#include "problems.h"
#include "domains.h"
#include "types.h"
#include <typeinfo>
#include <stack>
#include <iostream>


/*
 * A substitutes binary predicate.
 */
struct Substitutes
  : public std::binary_function<Substitution, const Variable*, bool> {
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
/* Term */

/* Constructs an abstract term with the given name. */
Term::Term(const std::string& name, const Type& type)
  : name_(name), type_(&type) {}


/* Deletes this term. */
Term::~Term() {
  const UnionType* ut = dynamic_cast<const UnionType*>(type_);
  if (ut != NULL) {
    delete ut;
  }
}


/* Adds the given type to the type of this term. */
void Term::add_type(const Type& type) {
  const Type& new_type = UnionType::add(*type_, type);
  const UnionType* ut = dynamic_cast<const UnionType*>(type_);
  if (ut != NULL) {
    delete ut;
  }
  type_ = &new_type;
}


/* Prints this term on the given stream with the given bindings. */
void Term::print(std::ostream& os, size_t step_id,
		 const Bindings& bindings) const {
  os << bindings.binding(*this, step_id);
}


/* Output operator for terms. */
std::ostream& operator<<(std::ostream& os, const Term& t) {
  os << t.name();
  return os;
}


/* ====================================================================== */
/* Name */

/* Constructs a name. */
Name::Name(const std::string& name, const Type& type)
  : Term(name, type) {}


/* Returns this term subject to the given substitutions. */
const Name& Name::substitution(const SubstitutionList& subst,
			       size_t step_id) const {
  return *this;
}


/* ====================================================================== */
/* Variable */

/* Constructs a variable with the given name and type. */
Variable::Variable(const std::string& name, const Type& type)
  : Term(name, type), ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_variables++;
#endif
}


/* Deletes this variable. */
Variable::~Variable() {
#ifdef DEBUG_MEMORY
  deleted_variables++;
#endif
}


/* Returns this term subject to the given substitutions. */
const Term& Variable::substitution(const SubstitutionList& subst,
				   size_t step_id) const {
  SubstListIter si = find_if(subst.begin(), subst.end(),
			     bind2nd(Substitutes(step_id), this));
  return (si != subst.end()) ? (*si).term() : *this;
}


/* ====================================================================== */
/* Formula */

/* The true formula. */
const Formula& Formula::TRUE = Constant::TRUE_;
/* The false formula. */
const Formula& Formula::FALSE = Constant::FALSE_;


/* Constructs a formula. */
Formula::Formula()
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_formulas++;
#endif
}


/* Deletes this formula. */
Formula::~Formula() {
#ifdef DEBUG_MEMORY
  deleted_formulas++;
#endif
}


/* Negation operator for formulas. */
const Formula& operator!(const Formula& f) {
  const Formula& neg = f.negation();
  Formula::register_use(&f);
  Formula::unregister_use(&f);
  return neg;
}


/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2) {
  if (f1.contradiction()) {
    Formula::register_use(&f2);
    Formula::unregister_use(&f2);
    return f1;
  } else if (f2.contradiction()) {
    Formula::register_use(&f1);
    Formula::unregister_use(&f1);
    return f2;
  } else if (f1.tautology()) {
    return f2;
  } else if (f2.tautology()) {
    return f1;
  } else {
    Conjunction& conjunction = *(new Conjunction());
    const Conjunction* c1 = dynamic_cast<const Conjunction*>(&f1);
    if (c1 != NULL) {
      for (FormulaListIter fi = c1->conjuncts().begin();
	   fi != c1->conjuncts().end(); fi++) {
	conjunction.add_conjunct(**fi);
      }
      Formula::register_use(c1);
      Formula::unregister_use(c1);
    } else {
      conjunction.add_conjunct(f1);
    }
    const Conjunction* c2 = dynamic_cast<const Conjunction*>(&f2);
    if (c2 != NULL) {
      for (FormulaListIter fi = c2->conjuncts().begin();
	   fi != c2->conjuncts().end(); fi++) {
	conjunction.add_conjunct(**fi);
      }
      Formula::register_use(c2);
      Formula::unregister_use(c2);
    } else {
      conjunction.add_conjunct(f2);
    }
    return conjunction;
  }
}


/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2) {
  if (f1.tautology()) {
    Formula::register_use(&f2);
    Formula::unregister_use(&f2);
    return f1;
  } else if (f2.tautology()) {
    Formula::register_use(&f1);
    Formula::unregister_use(&f1);
    return f2;
  } else if (f1.contradiction()) {
    return f2;
  } else if (f2.contradiction()) {
    return f1;
  } else {
    Disjunction& disjunction = *(new Disjunction());
    const Disjunction* d1 = dynamic_cast<const Disjunction*>(&f1);
    if (d1 != NULL) {
      for (FormulaListIter fi = d1->disjuncts().begin();
	   fi != d1->disjuncts().end(); fi++) {
	disjunction.add_disjunct(**fi);
      }
      Formula::register_use(d1);
      Formula::unregister_use(d1);
    } else {
      disjunction.add_disjunct(f1);
    }
    const Disjunction* d2 = dynamic_cast<const Disjunction*>(&f2);
    if (d2 != NULL) {
      for (FormulaListIter fi = d2->disjuncts().begin();
	   fi != d2->disjuncts().end(); fi++) {
	disjunction.add_disjunct(**fi);
      }
      Formula::register_use(d2);
      Formula::unregister_use(d2);
    } else {
      disjunction.add_disjunct(f2);
    }
    return disjunction;
  }
}


/* Output operator for formulas. */
std::ostream& operator<<(std::ostream& os, const Formula& f) {
  f.print(os);
  return os;
}


/* ====================================================================== */
/* Constant */

/* Constant representing true. */
const Constant Constant::TRUE_ = Constant(true);
/* Constant representing false. */
const Constant Constant::FALSE_ = Constant(false);


/* Constructs a constant formula. */
Constant::Constant(bool value)
  : value_(value) {
  register_use(this);
#ifdef DEBUG_MEMORY
  created_formulas--;
#endif
}


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


/* Prints this formula on the given stream with the given bindings. */
void Constant::print(std::ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  print(os);
}


/* Returns a negation of this formula. */
const Formula& Constant::negation() const {
  return value_ ? FALSE : TRUE;
}


/* Prints this object on the given stream. */
void Constant::print(std::ostream& os) const {
  os << (value_ ? "(and)" : "(or)");
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
      Disjunction* disj = NULL;
      const Formula* first_d = &FALSE;
      for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	const Substitution& subst = *si;
	if (subst.var() != subst.term()) {
	  const Formula& d = *(new Inequality(subst.var(), subst.term()));
	  if (first_d->contradiction()) {
	    first_d = &d;
	  } else if (disj == NULL) {
	    disj = new Disjunction();
	    disj->add_disjunct(*first_d);
	  }
	  if (disj != NULL) {
	    disj->add_disjunct(d);
	  }
	}
      }
      if (disj != NULL) {
	return *disj;
      } else {
	return *first_d;
      }
    }
  }
  return TRUE;
}


/* ====================================================================== */
/* Atom */

/* Constructs an atomic formula. */
Atom::Atom(const Predicate& predicate, FormulaTime when)
  : Literal(when), predicate_(&predicate) {}


/* Adds a term to this atomic formula. */
void Atom::add_term(const Term& term) {
  terms_.push_back(&term);
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const SubstitutionList& subst,
				   const Problem& problem) const {
  const Atom& f = substitution(subst, 0);
  if (problem.domain().static_predicate(predicate())) {
    const AtomList& adds = problem.init().add_list();
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      if (f == **gi) {
	register_use(&f);
	unregister_use(&f);
	return TRUE;
      } else if (Bindings::unifiable(f, 0, **gi, 0)) {
	return f;
      }
    }
    register_use(&f);
    unregister_use(&f);
    return FALSE;
  } else {
    return f;
  }
}


/* Returns this formula subject to the given substitutions. */
const Atom& Atom::substitution(const SubstitutionList& subst,
			       size_t step_id) const {
  if (terms().empty()) {
    return *this;
  } else {
    Atom& f = *(new Atom(predicate(), when()));
    for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
      f.add_term((*ti)->substitution(subst, step_id));
    }
    return f;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Atom::print(std::ostream& os, size_t step_id,
		 const Bindings& bindings) const {
  os << '(' << predicate().name();
  for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
    os << ' ';
    (*ti)->print(os, step_id, bindings);
  }
  os << ')';
}


/* Checks if this object equals the given object. */
bool Atom::equals(const Literal& o) const {
  const Atom* atom = dynamic_cast<const Atom*>(&o);
  return (atom != NULL && predicate() == atom->predicate()
	  && terms().size() == atom->terms().size()
	  && equal(terms().begin(), terms().end(), atom->terms().begin()));
}


/* Returns the hash value of this object. */
size_t Atom::hash_value() const {
  hashing::hash<const Term*> h;
  size_t val = size_t(&predicate());
  for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
    val = 5*val + h(*ti);
  }
  return val;
}


/* Returns the negation of this formula. */
const Literal& Atom::negation() const {
  return *(new Negation(*this));
}


/* Prints this object on the given stream. */
void Atom::print(std::ostream& os) const {
  os << '(' << predicate().name();
  for (TermListIter ti = terms().begin(); ti != terms().end(); ti++) {
    os << ' ' << **ti;
  }
  os << ')';
}


/* ====================================================================== */
/* Negation */

/* Constructs a negated atom. */
Negation::Negation(const Atom& atom)
  : Literal(atom.when()), atom_(&atom) {
  register_use(atom_);
}


/* Deletes this negated atom. */
Negation::~Negation() {
  unregister_use(atom_);
}


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


/* Prints this formula on the given stream with the given bindings. */
void Negation::print(std::ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  os << "(not ";
  atom().print(os, step_id, bindings);
  os << ")";
}


/* Checks if this object equals the given object. */
bool Negation::equals(const Literal& o) const {
  const Negation* negation = dynamic_cast<const Negation*>(&o);
  return negation != NULL && atom() == negation->atom();
}


/* Returns the hash value of this object. */
size_t Negation::hash_value() const {
  return 5*hashing::hash<const Literal*>()(&atom());
}


/* Returns the negation of this formula. */
const Literal& Negation::negation() const {
  return atom();
}


/* Prints this object on the given stream. */
void Negation::print(std::ostream& os) const {
  os << "(not " << atom() << ")";
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


/* Prints this formula on the given stream with the given bindings. */
void Equality::print(std::ostream& os, size_t step_id,
		     const Bindings& bindings) const {
  os << "(= ";
  term1().print(os, step_id, bindings);
  os << ' ';
  term2().print(os, step_id, bindings);
  os << ")";
}


/* Returns the negation of this formula. */
const BindingLiteral& Equality::negation() const {
  return *(new Inequality(term1(), step_id1(0), term2(), step_id2(0)));
}


/* Prints this object on the given stream. */
void Equality::print(std::ostream& os) const {
  os << "(= " << term1() << ' ' << term2() << ")";
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


/* Prints this formula on the given stream with the given bindings. */
void Inequality::print(std::ostream& os, size_t step_id,
		       const Bindings& bindings) const {
  os << "(not (= ";
  term1().print(os, step_id, bindings);
  os << ' ';
  term2().print(os, step_id, bindings);
  os << "))";
}


/* Returns the negation of this formula. */
const BindingLiteral& Inequality::negation() const {
  return *(new Equality(term1(), step_id1(0), term2(), step_id2(0)));
}


/* Prints this object on the given stream. */
void Inequality::print(std::ostream& os) const {
  os << "(not (= " << term1() << ' ' << term2() << "))";
}


/* ====================================================================== */
/* Conjunction */

/* Constructs an empty conjunction. */
Conjunction::Conjunction() {}


/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a conjunct to this conjunction. */
void Conjunction::add_conjunct(const Formula& conjunct) {
  conjuncts_.push_back(&conjunct);
  register_use(&conjunct);
}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Conjunction::separate(const Literal& literal) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->separate(literal);
    if (c.contradiction()) {
      if (conj == NULL) {
	register_use(first_c);
	unregister_use(first_c);
      } else {
	register_use(conj);
	unregister_use(conj);
      }
      return FALSE;
    } else if (!c.tautology()) {
      if (first_c->tautology()) {
	first_c = &c;
      } else if (conj == NULL) {
	conj = new Conjunction();
	conj->add_conjunct(*first_c);
      }
      if (conj != NULL) {
	conj->add_conjunct(c);
      }
    }
  }
  if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->instantiation(subst, problem);
    if (c.contradiction()) {
      if (conj == NULL) {
	register_use(first_c);
	unregister_use(first_c);
      } else {
	register_use(conj);
	unregister_use(conj);
      }
      return FALSE;
    } else if (!c.tautology()) {
      if (first_c->tautology()) {
	first_c = &c;
      } else if (conj == NULL) {
	conj = new Conjunction();
	conj->add_conjunct(*first_c);
      }
      if (conj != NULL) {
	conj->add_conjunct(c);
      }
    }
  }
  if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Returns this formula subject to the given substitutions. */
const Formula& Conjunction::substitution(const SubstitutionList& subst,
					 size_t step_id) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->substitution(subst, step_id);
    if (c.contradiction()) {
      if (conj == NULL) {
	register_use(first_c);
	unregister_use(first_c);
      } else {
	register_use(conj);
	unregister_use(conj);
      }
      return FALSE;
    } else if (!c.tautology()) {
      if (first_c->tautology()) {
	first_c = &c;
      } else if (conj == NULL) {
	conj = new Conjunction();
	conj->add_conjunct(*first_c);
      }
      if (conj != NULL) {
	conj->add_conjunct(c);
      }
    }
  }
  if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Conjunction::print(std::ostream& os, size_t step_id,
			const Bindings& bindings) const {
  os << "(and";
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, step_id, bindings);
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Conjunction::negation() const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& d = !**fi;
    if (d.tautology()) {
      if (disj == NULL) {
	register_use(first_d);
	unregister_use(first_d);
      } else {
	register_use(disj);
	unregister_use(disj);
      }
      return TRUE;
    } else if (!d.contradiction()) {
      if (first_d->contradiction()) {
	first_d = &d;
      } else if (disj == NULL) {
	disj = new Disjunction();
	disj->add_disjunct(*first_d);
      }
      if (disj != NULL) {
	disj->add_disjunct(d);
      }
    }
  }
  if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Prints this object on the given stream. */
void Conjunction::print(std::ostream& os) const {
  os << "(and";
  for (FormulaListIter fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* ====================================================================== */
/* Disjunction */

/* Constructs an empty disjunction. */
Disjunction::Disjunction() {}


/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    unregister_use(*fi);
  }
}


/* Adds a disjunct to this disjunction. */
void Disjunction::add_disjunct(const Formula& disjunct) {
  disjuncts_.push_back(&disjunct);
  register_use(&disjunct);
}


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
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = (*fi)->instantiation(subst, problem);
    if (d.tautology()) {
      if (disj == NULL) {
	register_use(first_d);
	unregister_use(first_d);
      } else {
	register_use(disj);
	unregister_use(disj);
      }
      return TRUE;
    } else if (!d.contradiction()) {
      if (first_d->contradiction()) {
	first_d = &d;
      } else if (disj == NULL) {
	disj = new Disjunction();
	disj->add_disjunct(*first_d);
      }
      if (disj != NULL) {
	disj->add_disjunct(d);
      }
    }
  }
  if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Returns this formula subject to the given substitutions. */
const Formula& Disjunction::substitution(const SubstitutionList& subst,
					 size_t step_id) const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = (*fi)->substitution(subst, step_id);
    if (d.tautology()) {
      if (disj == NULL) {
	register_use(first_d);
	unregister_use(first_d);
      } else {
	register_use(disj);
	unregister_use(disj);
      }
      return TRUE;
    } else if (!d.contradiction()) {
      if (first_d->contradiction()) {
	first_d = &d;
      } else if (disj == NULL) {
	disj = new Disjunction();
	disj->add_disjunct(*first_d);
      }
      if (disj != NULL) {
	disj->add_disjunct(d);
      }
    }
  }
  if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Disjunction::print(std::ostream& os, size_t step_id,
			const Bindings& bindings) const {
  os << "(or";
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, step_id, bindings);
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Disjunction::negation() const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& c = !**fi;
    if (c.contradiction()) {
      if (conj == NULL) {
	register_use(first_c);
	unregister_use(first_c);
      } else {
	register_use(conj);
	unregister_use(conj);
      }
      return FALSE;
    } else if (!c.tautology()) {
      if (first_c->tautology()) {
	first_c = &c;
      } else if (conj == NULL) {
	conj = new Conjunction();
	conj->add_conjunct(*first_c);
      }
      if (conj != NULL) {
	conj->add_conjunct(c);
      }
    }
  }
  if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Prints this object on the given stream. */
void Disjunction::print(std::ostream& os) const {
  os << "(or";
  for (FormulaListIter fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    os << ' ' << **fi;
  }
  os << ")";
}


/* ====================================================================== */
/* QuantifiedFormula */

/* Constructs a quantified formula. */
QuantifiedFormula::QuantifiedFormula(const Formula& body)
  : body_(&body) {
  register_use(body_);
}


/* Deletes this quantified formula. */
QuantifiedFormula::~QuantifiedFormula() {
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    Variable::unregister_use(*vi);
  }
  unregister_use(body_);
}


/* Adds a quantified variable to this quantified formula. */
void QuantifiedFormula::add_parameter(const Variable& parameter) {
  parameters_.push_back(&parameter);
  Variable::register_use(&parameter);
}


/* Sets the body of this quantified formula. */
void QuantifiedFormula::set_body(const Formula& body) {
  if (body_ != &body) {
    unregister_use(body_);
    body_ = &body;
    register_use(body_);
  }
}


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
ExistsFormula::ExistsFormula()
  : QuantifiedFormula(FALSE) {}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  size_t n = parameters().size();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = *si;
      if (find(parameters().begin(), parameters().end(), &s.var())
	  == parameters().end()) {
	args.push_back(s);
      }
    }
    std::vector<NameList> arguments(n, NameList());
    std::vector<NameListIter> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i], parameters()[i]->type());
      if (arguments[i].empty()) {
	return FALSE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    const Formula* result = &FALSE;
    std::stack<const Formula*> disjuncts;
    disjuncts.push(&body().instantiation(args, problem));
    register_use(disjuncts.top());
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
	  if (j < i) {
	    unregister_use(disjuncts.top());
	  }
	  disjuncts.pop();
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
	    }
	  } else {
	    i = j;
	    break;
	  }
	}
      } else {
	register_use(disjuncts.top());
	i++;
      }
    }
    while (!disjuncts.empty()) {
      unregister_use(disjuncts.top());
      disjuncts.pop();
    }
    return *result;
  }
}


/* Returns this formula subject to the given substitutions. */
const Formula& ExistsFormula::substitution(const SubstitutionList& subst,
					   size_t step_id) const {
  const Formula& b = body().substitution(subst, step_id);
  if (b.constant()) {
    return b;
  } else {
    ExistsFormula& exists = *(new ExistsFormula());
    for (VarListIter vi = parameters().begin();
	 vi != parameters().end(); vi++) {
      exists.add_parameter(**vi);
    }
    exists.set_body(b);
    return exists;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void ExistsFormula::print(std::ostream& os, size_t step_id,
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


/* Returns the negation of this formula. */
const QuantifiedFormula& ExistsFormula::negation() const {
  ForallFormula& forall = *(new ForallFormula());
  for (VarListIter vi = parameters().begin();
       vi != parameters().end(); vi++) {
    forall.add_parameter(**vi);
  }
  forall.set_body(!body());
  return forall;
}


/* Prints this object on the given stream. */
void ExistsFormula::print(std::ostream& os) const {
  os << "(exists (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << **vi << " - " << (*vi)->type();
  }
  os << ") " << body() << ")";
}


/* ====================================================================== */
/* ForallFormula */

/* Constructs a universally quantified formula. */
ForallFormula::ForallFormula()
  : QuantifiedFormula(TRUE) {}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  size_t n = parameters().size();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = *si;
      if (find(parameters().begin(), parameters().end(), &s.var())
	  == parameters().end()) {
	args.push_back(s);
      }
    }
    std::vector<NameList> arguments(n, NameList());
    std::vector<NameListIter> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i], parameters()[i]->type());
      if (arguments[i].empty()) {
	return TRUE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    const Formula* result = &TRUE;
    std::stack<const Formula*> conjuncts;
    conjuncts.push(&body().instantiation(args, problem));
    register_use(conjuncts.top());
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
	  if (j < i) {
	    unregister_use(conjuncts.top());
	  }
	  conjuncts.pop();
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
	    }
	  } else {
	    i = j;
	    break;
	  }
	}
      } else {
	register_use(conjuncts.top());
	i++;
      }
    }
    while (!conjuncts.empty()) {
      unregister_use(conjuncts.top());
      conjuncts.pop();
    }
    return *result;
  }
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ForallFormula::substitution(const SubstitutionList& subst,
			    size_t step_id) const {
  const Formula& b = body().substitution(subst, step_id);
  if (b.constant()) {
    return b;
  } else {
    ForallFormula& forall = *(new ForallFormula());
    for (VarListIter vi = parameters().begin();
	 vi != parameters().end(); vi++) {
      forall.add_parameter(**vi);
    }
    forall.set_body(b);
    return forall;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void ForallFormula::print(std::ostream& os, size_t step_id,
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


/* Returns the negation of this formula. */
const QuantifiedFormula& ForallFormula::negation() const {
  ExistsFormula& exists = *(new ExistsFormula());
  for (VarListIter vi = parameters().begin();
       vi != parameters().end(); vi++) {
    exists.add_parameter(**vi);
  }
  exists.set_body(!body());
  return exists;
}


/* Prints this object on the given stream. */
void ForallFormula::print(std::ostream& os) const {
  os << "(forall (";
  for (VarListIter vi = parameters().begin(); vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << **vi << " - " << (*vi)->type();
  }
  os << ") " << body() << ")";
}
