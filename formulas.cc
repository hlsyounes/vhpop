/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: formulas.cc,v 6.7 2003-08-24 18:42:26 lorens Exp $
 */
#include "formulas.h"
#include "bindings.h"
#include "problems.h"
#include "domains.h"
#include <stack>
#include <iostream>


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
  } else if (&f1 == &f2) {
    return f1;
  } else {
    Conjunction& conjunction = *new Conjunction();
    const Conjunction* c1 = dynamic_cast<const Conjunction*>(&f1);
    if (c1 != NULL) {
      for (FormulaList::const_iterator fi = c1->conjuncts().begin();
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
      for (FormulaList::const_iterator fi = c2->conjuncts().begin();
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
  } else if (&f1 == &f2) {
    return f1;
  } else {
    Disjunction& disjunction = *new Disjunction();
    const Disjunction* d1 = dynamic_cast<const Disjunction*>(&f1);
    if (d1 != NULL) {
      for (FormulaList::const_iterator fi = d1->disjuncts().begin();
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
      for (FormulaList::const_iterator fi = d2->disjuncts().begin();
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
const Formula& Constant::separator(const Literal& literal) const {
  return TRUE;
}


/* Returns this formula subject to the given substitutions. */
const Constant& Constant::substitution(const SubstitutionMap& subst) const {
  return *this;
}


/* Returns an instantiation of this formula. */
const Constant& Constant::instantiation(const SubstitutionMap& subst,
					const Problem& problem) const {
  return *this;
}


/* Returns the universal base of this formula. */
const Formula& Constant::universal_base(const SubstitutionMap& subst,
					const Problem& problem) const {
  return *this;
}


/* Prints this formula on the given stream with the given bindings. */
void Constant::print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const {
  os << (value_ ? "(and)" : "(or)");
}


/* Returns a negation of this formula. */
const Formula& Constant::negation() const {
  return value_ ? FALSE : TRUE;
}


/* ====================================================================== */
/* Literal */

/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Literal::separator(const Literal& literal) const {
  BindingList mgu;
  if (Bindings::unifiable(mgu, *this, 0, literal, 0)) {
    Disjunction* disj = NULL;
    const Formula* first_d = &FALSE;
    for (BindingList::const_iterator bi = mgu.begin(); bi != mgu.end(); bi++) {
      const Binding& b = *bi;
      if (b.var() != b.term()) {
	const Formula& d = Inequality::make(b.var(), b.term());
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
  return TRUE;
}


/* ====================================================================== */
/* Atom */

/* Table of atomic formulas. */
Atom::AtomTable Atom::atoms;


/* Comparison function. */
bool Atom::AtomLess::operator()(const Atom* a1, const Atom* a2) const {
  if (a1->predicate() < a2->predicate()) {
    return true;
  } else if (a1->predicate() > a2->predicate()) {
    return false;
  } else {
    for (size_t i = 0; i < a1->arity(); i++) {
      if (a1->term(i) < a2->term(i)) {
	return true;
      } else if (a1->term(i) > a2->term(i)) {
	return false;
      }
    }
    return false;
  }
}


/* Returns an atomic state formula with the given predicate and terms. */
const Atom& Atom::make(Predicate predicate, const TermList& terms) {
  Atom* atom = new Atom(predicate);
  bool ground = true;
  for (TermList::const_iterator ti = terms.begin(); ti != terms.end(); ti++) {
    atom->add_term(*ti);
    if (ground && is_variable(*ti)) {
      ground = false;
    }
  }
  if (!ground) {
    return *atom;
  } else {
    std::pair<AtomTable::const_iterator, bool> result = atoms.insert(atom);
    if (!result.second) {
      delete atom;
      return **result.first;
    } else {
      return *atom;
    }
  }
}


/* Constructs an atomic formula with the given predicate. */
Atom::Atom(Predicate predicate)
  : predicate_(predicate) {}


/* Deletes this atomic formula. */
Atom::~Atom() {
  AtomTable::const_iterator ai = atoms.find(this);
  if (*ai == this) {
    atoms.erase(ai);
  }
}


/* Returns this formula subject to the given substitutions. */
const Atom& Atom::substitution(const SubstitutionMap& subst) const {
  TermList inst_terms;
  bool substituted = false;
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    SubstitutionMap::const_iterator si =
      is_variable(*ti) ? subst.find(*ti) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
    } else {
      inst_terms.push_back(*ti);
    }
  }
  if (substituted) {
    return make(predicate(), inst_terms);
  } else {
    return *this;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Atom::instantiation(const SubstitutionMap& subst,
				   const Problem& problem) const {
  const Atom& f = substitution(subst);
  if (problem.domain().predicates().static_predicate(predicate())) {
    const AtomList& adds = problem.init();
    for (AtomList::const_iterator gi = adds.begin(); gi != adds.end(); gi++) {
      if (&f == *gi) {
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


/* Returns the universal base of this formula. */
const Formula& Atom::universal_base(const SubstitutionMap& subst,
				    const Problem& problem) const {
  return instantiation(subst, problem);
}


/* Prints this formula on the given stream with the given bindings. */
void Atom::print(std::ostream& os, const PredicateTable& predicates,
		 const TermTable& terms, size_t step_id,
		 const Bindings& bindings) const {
  os << '(';
  predicates.print_predicate(os, predicate());
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    os << ' ';
    terms.print_term(os, *ti, step_id, bindings);
  }
  os << ')';
}


/* Returns the negation of this formula. */
const Literal& Atom::negation() const {
  return Negation::make(*this);
}


/* ====================================================================== */
/* Negation */

/* Table of negated atoms. */
Negation::NegationTable Negation::negations;


/* Comparison function. */
bool Negation::NegationLess::operator()(const Negation* n1,
					const Negation* n2) const {
  return &n1->atom() < &n2->atom();
}


/* Returns a negation of the given atom. */
const Negation& Negation::make(const Atom& atom) {
  Negation* negation = new Negation(atom);
  bool ground = true;
  size_t n = atom.arity();
  for (size_t i = 0; i < n; i++) {
    if (ground && is_variable(atom.term(i))) {
      ground = false;
    }
  }
  if (!ground) {
    return *negation;
  } else {
    std::pair<NegationTable::const_iterator, bool> result =
      negations.insert(negation);
    if (!result.second) {
      delete negation;
      return **result.first;
    } else {
      return *negation;
    }
  }
}


/* Constructs a negated atom. */
Negation::Negation(const Atom& atom)
  : atom_(&atom) {
  register_use(atom_);
}


/* Deletes this negated atom. */
Negation::~Negation() {
  unregister_use(atom_);
  NegationTable::const_iterator ni = negations.find(this);
  if (*ni == this) {
    negations.erase(ni);
  }
}


/* Returns this formula subject to the given substitutions. */
const Negation& Negation::substitution(const SubstitutionMap& subst) const {
  const Atom& f = atom().substitution(subst);
  if (&f == atom_) {
    return *this;
  } else {
    return make(f);
  }
}


/* Returns an instantiation of this formula. */
const Formula& Negation::instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const {
  const Formula& f = atom().instantiation(subst, problem);
  if (&f == atom_) {
    return *this;
  } else {
    return !f;
  }
}


/* Returns the universal base of this formula. */
const Formula& Negation::universal_base(const SubstitutionMap& subst,
					const Problem& problem) const {
  return instantiation(subst, problem);
}


/* Prints this formula on the given stream with the given bindings. */
void Negation::print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const {
  os << "(not ";
  atom().print(os, predicates, terms, step_id, bindings);
  os << ")";
}


/* Returns the negation of this formula. */
const Literal& Negation::negation() const {
  return atom();
}


/* ====================================================================== */
/* BindingLiteral */

/* Constructs a binding literal. */
BindingLiteral::BindingLiteral(Term term1, size_t id1,
			       Term term2, size_t id2)
  : term1_(term1), id1_(id1), term2_(term2), id2_(id2) {}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& BindingLiteral::separator(const Literal& literal) const {
  return TRUE;
}


/* ====================================================================== */
/* Equality */

/* Returns an equality of the two terms. */
const Formula& Equality::make(Term term1, Term term2) {
  if (term1 == term2) {
    return TRUE;
  } else if (is_object(term1) && is_object(term2) && term1 != term2) {
    return FALSE;
  } else {
    return *new Equality(term1, 0, term2, 0);
  }
}


/* Returns an equality of the two terms. */
const Formula& Equality::make(Term term1, size_t id1, Term term2, size_t id2) {
  if (term1 == term2 && id1 == id2) {
    return TRUE;
  } else if (is_object(term1) && is_object(term2)) {
    return (term1 == term2) ? TRUE : FALSE;
  } else {
    return *new Equality(term1, id1, term2, id2);
  }
}


/* Constructs an equality with assigned step ids. */
Equality::Equality(Term term1, size_t id1, Term term2, size_t id2)
  : BindingLiteral(term1, id1, term2, id2) {}


/* Returns this formula subject to the given substitutions. */
const Formula& Equality::substitution(const SubstitutionMap& subst) const {
  Term t1 = term1();
  Term t2 = term2();
  if (is_variable(t1)) {
    const SubstitutionMap::const_iterator si = subst.find(t1);
    if (si != subst.end()) {
      t1 = (*si).second;
    }
  }
  if (is_variable(t2)) {
    const SubstitutionMap::const_iterator si = subst.find(t2);
    if (si != subst.end()) {
      t2 = (*si).second;
    }
  }
  if (t1 == term1() && t2 == term2()) {
    return *this;
  } else {
    return make(t1, step_id1(0), t2, step_id2(0));
  }
}


/* Returns an instantiation of this formula. */
const Formula& Equality::instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const {
  return substitution(subst);
}


/* Returns the universal base of this formula. */
const Formula& Equality::universal_base(const SubstitutionMap& subst,
					const Problem& problem) const {
  return substitution(subst);
}


/* Prints this formula on the given stream with the given bindings. */
void Equality::print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const {
  os << "(= ";
  terms.print_term(os, term1(), step_id, bindings);
  os << ' ';
  terms.print_term(os, term2(), step_id, bindings);
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Equality::negation() const {
  return Inequality::make(term1(), step_id1(0), term2(), step_id2(0));
}


/* ====================================================================== */
/* Inequality */

/* Returns an equality of the two terms. */
const Formula& Inequality::make(Term term1, Term term2) {
  if (term1 == term2) {
    return FALSE;
  } else if (is_object(term1) && is_object(term2) && term1 != term2) {
    return TRUE;
  } else {
    return *new Inequality(term1, 0, term2, 0);
  }
}


/* Returns an equality of the two terms. */
const Formula& Inequality::make(Term term1, size_t id1,
				Term term2, size_t id2) {
  if (term1 == term2 && id1 == id2) {
    return FALSE;
  } else if (is_object(term1) && is_object(term2)) {
    return (term1 != term2) ? TRUE : FALSE;
  } else {
    return *new Inequality(term1, id1, term2, id2);
  }
}


/* Constructs an inequality with assigned step ids. */
Inequality::Inequality(Term term1, size_t id1, Term term2, size_t id2)
  : BindingLiteral(term1, id1, term2, id2) {}


/* Returns this formula subject to the given substitutions. */
const Formula& Inequality::substitution(const SubstitutionMap& subst) const {
  Term t1 = term1();
  Term t2 = term2();
  if (is_variable(t1)) {
    const SubstitutionMap::const_iterator si = subst.find(t1);
    if (si != subst.end()) {
      t1 = (*si).second;
    }
  }
  if (is_variable(t2)) {
    const SubstitutionMap::const_iterator si = subst.find(t2);
    if (si != subst.end()) {
      t2 = (*si).second;
    }
  }
  if (t1 == term1() && t2 == term2()) {
    return *this;
  } else {
    return make(t1, step_id1(0), t2, step_id2(0));
  }
}


/* Returns an instantiation of this formula. */
const Formula& Inequality::instantiation(const SubstitutionMap& subst,
					 const Problem& problem) const {
  return substitution(subst);
}


/* Returns the universal base of this formula. */
const Formula& Inequality::universal_base(const SubstitutionMap& subst,
					  const Problem& problem) const {
  return substitution(subst);
}


/* Prints this formula on the given stream with the given bindings. */
void Inequality::print(std::ostream& os, const PredicateTable& predicates,
		       const TermTable& terms, size_t step_id,
		       const Bindings& bindings) const {
  os << "(not (= ";
  terms.print_term(os, term1(), step_id, bindings);
  os << ' ';
  terms.print_term(os, term2(), step_id, bindings);
  os << "))";
}


/* Returns the negation of this formula. */
const Formula& Inequality::negation() const {
  return Equality::make(term1(), step_id1(0), term2(), step_id2(0));
}


/* ====================================================================== */
/* Conjunction */

/* Constructs an empty conjunction. */
Conjunction::Conjunction() {}


/* Deletes this conjunction. */
Conjunction::~Conjunction() {
  for (FormulaList::const_iterator fi = conjuncts().begin();
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
const Formula& Conjunction::separator(const Literal& literal) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->separator(literal);
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
const Formula& Conjunction::substitution(const SubstitutionMap& subst) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  bool changed = false;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->substitution(subst);
    if (&c != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (conj == NULL) {
      register_use(first_c);
      unregister_use(first_c);
    } else {
      register_use(conj);
      unregister_use(conj);
    }
    return *this;
  } else if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  bool changed = false;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->instantiation(subst, problem);
    if (&c != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (conj == NULL) {
      register_use(first_c);
      unregister_use(first_c);
    } else {
      register_use(conj);
      unregister_use(conj);
    }
    return *this;
  } else if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Returns the universal base of this formula. */
const Formula& Conjunction::universal_base(const SubstitutionMap& subst,
					   const Problem& problem) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  bool changed = false;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    const Formula& c = (*fi)->universal_base(subst, problem);
    if (&c != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (conj == NULL) {
      register_use(first_c);
      unregister_use(first_c);
    } else {
      register_use(conj);
      unregister_use(conj);
    }
    return *this;
  } else if (conj != NULL) {
    return *conj;
  } else {
    return *first_c;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Conjunction::print(std::ostream& os, const PredicateTable& predicates,
			const TermTable& terms, size_t step_id,
			const Bindings& bindings) const {
  os << "(and";
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, predicates, terms, step_id, bindings);
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Conjunction::negation() const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  for (FormulaList::const_iterator fi = conjuncts().begin();
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


/* ====================================================================== */
/* Disjunction */

/* Constructs an empty disjunction. */
Disjunction::Disjunction() {}


/* Deletes this disjunction. */
Disjunction::~Disjunction() {
  for (FormulaList::const_iterator fi = disjuncts().begin();
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
const Formula& Disjunction::separator(const Literal& literal) const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = **fi;
    const Formula& c = !d && d.separator(literal);
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
const Formula& Disjunction::substitution(const SubstitutionMap& subst) const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  bool changed = false;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = (*fi)->substitution(subst);
    if (&d != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (disj == NULL) {
      register_use(first_d);
      unregister_use(first_d);
    } else {
      register_use(disj);
      unregister_use(disj);
    }
    return *this;
  } else if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  bool changed = false;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = (*fi)->instantiation(subst, problem);
    if (&d != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (disj == NULL) {
      register_use(first_d);
      unregister_use(first_d);
    } else {
      register_use(disj);
      unregister_use(disj);
    }
    return *this;
  } else if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Returns the universal base of this formula. */
const Formula& Disjunction::universal_base(const SubstitutionMap& subst,
					   const Problem& problem) const {
  Disjunction* disj = NULL;
  const Formula* first_d = &FALSE;
  bool changed = false;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    const Formula& d = (*fi)->universal_base(subst, problem);
    if (&d != *fi) {
      changed = true;
    }
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
  if (!changed) {
    if (disj == NULL) {
      register_use(first_d);
      unregister_use(first_d);
    } else {
      register_use(disj);
      unregister_use(disj);
    }
    return *this;
  } else if (disj != NULL) {
    return *disj;
  } else {
    return *first_d;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Disjunction::print(std::ostream& os, const PredicateTable& predicates,
			const TermTable& terms, size_t step_id,
			const Bindings& bindings) const {
  os << "(or";
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end(); fi++) {
    os << ' ';
    (*fi)->print(os, predicates, terms, step_id, bindings);
  }
  os << ")";
}


/* Returns the negation of this formula. */
const Formula& Disjunction::negation() const {
  Conjunction* conj = NULL;
  const Formula* first_c = &TRUE;
  for (FormulaList::const_iterator fi = disjuncts().begin();
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


/* ====================================================================== */
/* Quantification */

/* Constructs a quantified formula. */
Quantification::Quantification(const Formula& body)
  : body_(&body) {
  register_use(body_);
}


/* Deletes this quantified formula. */
Quantification::~Quantification() {
  unregister_use(body_);
}


/* Adds a quantified variable to this quantified formula. */
void Quantification::add_parameter(Variable parameter) {
  parameters_.push_back(parameter);
}


/* Sets the body of this quantified formula. */
void Quantification::set_body(const Formula& body) {
  if (body_ != &body) {
    unregister_use(body_);
    body_ = &body;
    register_use(body_);
  }
}


/* Returns a formula that separates the given literal from anything
   definitely asserted by this formula. */
const Formula& Quantification::separator(const Literal& literal) const {
  /* We are being conservative here.  It can be hard to find a
     separator in this case. */
  return TRUE;
}


/* ====================================================================== */
/* Exists */

/* Constructs an existentially quantified formula. */
Exists::Exists()
  : Quantification(FALSE) {}


/* Returns this formula subject to the given substitutions. */
const Formula& Exists::substitution(const SubstitutionMap& subst) const {
  const Formula& b = body().substitution(subst);
  if (&b == &body()) {
    return *this;
  } else if (b.tautology() || b.contradiction()) {
    return b;
  } else {
    Exists& exists = *new Exists();
    for (VariableList::const_iterator vi = parameters().begin();
	 vi != parameters().end(); vi++) {
      exists.add_parameter(*vi);
    }
    exists.set_body(b);
    return exists;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Exists::instantiation(const SubstitutionMap& subst,
				     const Problem& problem) const {
  int n = parameters().size();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameters()[i]));
      if (arguments[i].empty()) {
	return FALSE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    const Formula* result = &FALSE;
    std::stack<const Formula*> disjuncts;
    disjuncts.push(&body().instantiation(args, problem));
    register_use(disjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
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


/* Returns the universal base of this formula. */
const Formula& Exists::universal_base(const SubstitutionMap& subst,
				      const Problem& problem) const {
  const Formula& b = body().universal_base(subst, problem);
  if (&b == &body()) {
    return *this;
  } else if (b.tautology() || b.contradiction()) {
    return b;
  } else {
    Exists& exists = *new Exists();
    for (VariableList::const_iterator vi = parameters().begin();
	 vi != parameters().end(); vi++) {
      exists.add_parameter(*vi);
    }
    exists.set_body(b);
    return exists;
  }
}


/* Prints this formula on the given stream with the given bindings. */
void Exists::print(std::ostream& os, const PredicateTable& predicates,
		   const TermTable& terms, size_t step_id,
		   const Bindings& bindings) const {
  os << "(exists (";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    terms.print_term(os, *vi, step_id, bindings);
  }
  os << ") ";
  body().print(os, predicates, terms, step_id, bindings);
  os << ")";
}


/* Returns the negation of this formula. */
const Quantification& Exists::negation() const {
  Forall& forall = *new Forall();
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    forall.add_parameter(*vi);
  }
  forall.set_body(!body());
  return forall;
}


/* ====================================================================== */
/* Forall */

/* Constructs a universally quantified formula. */
Forall::Forall()
  : Quantification(TRUE), universal_base_(NULL) {}


/* Returns this formula subject to the given substitutions. */
const Formula& Forall::substitution(const SubstitutionMap& subst) const {
  const Formula& b = body().substitution(subst);
  if (&b == &body()) {
    return *this;
  } else if (b.tautology() || b.contradiction()) {
    return b;
  } else {
    Forall& forall = *new Forall();
    for (VariableList::const_iterator vi = parameters().begin();
	 vi != parameters().end(); vi++) {
      forall.add_parameter(*vi);
    }
    forall.set_body(b);
    return forall;
  }
}


/* Returns an instantiation of this formula. */
const Formula& Forall::instantiation(const SubstitutionMap& subst,
				     const Problem& problem) const {
  int n = parameters().size();
  if (n == 0) {
    return body().instantiation(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameters()[i]));
      if (arguments[i].empty()) {
	return TRUE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    const Formula* result = &TRUE;
    std::stack<const Formula*> conjuncts;
    conjuncts.push(&body().instantiation(args, problem));
    register_use(conjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
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


/* Returns the universal base of this formula. */
const Formula& Forall::universal_base(const SubstitutionMap& subst,
				      const Problem& problem) const {
  if (universal_base_ != NULL) {
    return *universal_base_;
  }
  int n = parameters().size();
  if (n == 0) {
    universal_base_ = &body().universal_base(subst, problem);
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (int i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.terms().type(parameters()[i]));
      if (arguments[i].empty()) {
	universal_base_ = &TRUE;
	return TRUE;
      }
      next_arg.push_back(arguments[i].begin());
    }
    universal_base_ = &TRUE;
    std::stack<const Formula*> conjuncts;
    conjuncts.push(&body().universal_base(args, problem));
    register_use(conjuncts.top());
    for (int i = 0; i < n; ) {
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Formula& conjunct =
	conjuncts.top()->universal_base(pargs, problem);
      conjuncts.push(&conjunct);
      if (i + 1 == n) {
	universal_base_ = &(*universal_base_ && conjunct);
	if (universal_base_->contradiction()) {
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
  }
  return *universal_base_;
}


/* Prints this formula on the given stream with the given bindings. */
void Forall::print(std::ostream& os, const PredicateTable& predicates,
		   const TermTable& terms, size_t step_id,
		   const Bindings& bindings) const {
  os << "(forall (";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    terms.print_term(os, *vi, step_id, bindings);
  }
  os << ") ";
  body().print(os, predicates, terms, step_id, bindings);
  os << ")";
}


/* Returns the negation of this formula. */
const Quantification& Forall::negation() const {
  Exists& exists = *new Exists();
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    exists.add_parameter(*vi);
  }
  exists.set_body(!body());
  return exists;
}


/* ====================================================================== */
/* Condition */

/* The true condition. */
const Condition Condition::TRUE = Condition(true);
/* The false condition. */
const Condition Condition::FALSE = Condition(false);


/* Returns a condition. */
const Condition& Condition::make(const Formula& at_start,
				 const Formula& over_all,
				 const Formula& at_end) {
  if (at_start.tautology() && over_all.tautology() && at_end.tautology()) {
    Formula::register_use(&at_start);
    Formula::unregister_use(&at_start);
    Formula::register_use(&over_all);
    Formula::unregister_use(&over_all);
    Formula::register_use(&at_end);
    Formula::unregister_use(&at_end);
    return TRUE;
  } else if (at_start.contradiction() || over_all.contradiction()
	     || at_end.contradiction()) {
    Formula::register_use(&at_start);
    Formula::unregister_use(&at_start);
    Formula::register_use(&over_all);
    Formula::unregister_use(&over_all);
    Formula::register_use(&at_end);
    Formula::unregister_use(&at_end);
    return FALSE;
  } else {
    return *new Condition(at_start, over_all, at_end);
  }
}


/* Returns a condition. */
const Condition& Condition::make(const Formula& formula, FormulaTime when) {
  switch (when) {
  default:
  case AT_START:
    return make(formula, Formula::TRUE, Formula::TRUE);
  case OVER_ALL:
    return make(Formula::TRUE, formula, Formula::TRUE);
  case AT_END:
    return make(Formula::TRUE, Formula::TRUE, formula);
  }
}


/* Constructs a true condition. */
Condition::Condition(bool b)
  : ref_count_(0) {
  if (b) {
    at_start_ = &Formula::TRUE;
    over_all_ = &Formula::TRUE;
    at_end_ = &Formula::TRUE;
  } else {
    at_start_ = &Formula::FALSE;
    over_all_ = &Formula::FALSE;
    at_end_ = &Formula::FALSE;
  }
  Formula::register_use(at_start_);
  Formula::register_use(over_all_);
  Formula::register_use(at_end_);
  register_use(this);
}


/* Constructs a condition. */
Condition::Condition(const Formula& at_start, const Formula& over_all,
		     const Formula& at_end)
  : at_start_(&at_start), over_all_(&over_all), at_end_(&at_end),
    ref_count_(0) {
  Formula::register_use(at_start_);
  Formula::register_use(over_all_);
  Formula::register_use(at_end_);
#ifdef DEBUG_MEMORY
  created_conditions++;
#endif
}


/* Deletes this condition. */
Condition::~Condition() {
  Formula::unregister_use(at_start_);
  Formula::unregister_use(over_all_);
  Formula::unregister_use(at_end_);
#ifdef DEBUG_MEMORY
  deleted_conditions++;
#endif
}


/* Returns this condition subject to the given substitutions. */
const Condition& Condition::substitution(const SubstitutionMap& subst) const {
  const Formula& f1 = at_start().substitution(subst);
  const Formula& f2 = over_all().substitution(subst);
  const Formula& f3 = at_end().substitution(subst);
  if (&f1 == at_start_ && &f2 == over_all_ && &f3 == at_end_) {
    return *this;
  } else {
    return make(f1, f2, f3);
  }
}


/* Returns an instantiation of this condition. */
const Condition& Condition::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  const Formula& f1 = at_start().instantiation(subst, problem);
  const Formula& f2 = over_all().instantiation(subst, problem);
  const Formula& f3 = at_end().instantiation(subst, problem);
  if (&f1 == at_start_ && &f2 == over_all_ && &f3 == at_end_) {
    return *this;
  } else {
    return make(f1, f2, f3);
  }
}


/* Prints this condition on the given stream with the given bindings. */
void Condition::print(std::ostream& os, const PredicateTable& predicates,
		      const TermTable& terms, size_t step_id,
		      const Bindings& bindings) const {
  if (tautology()) {
    os << "(and)";
  } else if (contradiction()) {
    os << "(or)";
  } else {
    size_t n = 0;
    if (!at_start().tautology()) {
      n++;
    }
    if (!over_all().tautology()) {
      n++;
    }
    if (!at_end().tautology()) {
      n++;
    }
    if (n > 1) {
      os << "(and";
    }
    if (!at_start().tautology()) {
      if (n > 1) {
	os << ' ';
      }
      os << "(at start ";
      at_start().print(os, predicates, terms, step_id, bindings);
      os << ")";
    }
    if (!over_all().tautology()) {
      if (n > 1) {
	os << ' ';
      }
      os << "(over all ";
      over_all().print(os, predicates, terms, step_id, bindings);
      os << ")";
    }
    if (!at_end().tautology()) {
      if (n > 1) {
	os << ' ';
      }
      os << "(at end ";
      at_end().print(os, predicates, terms, step_id, bindings);
      os << ")";
    }
    if (n > 1) {
      os << ")";
    }
  }
}


/* Negation operator for conditions. */
const Condition& operator!(const Condition& c) {
  const Formula& at_start = !c.at_start();
  const Formula& over_all = !c.over_all();
  const Formula& at_end = !c.at_end();
  const Condition& cond = Condition::make(at_start, over_all, at_end);
  Condition::register_use(&c);
  Condition::unregister_use(&c);
  return cond;
}


/* Conjunction operator for conditions. */
const Condition& operator&&(const Condition& c1, const Condition& c2) {
  const Formula& at_start = c1.at_start() && c2.at_start();
  const Formula& over_all = c1.over_all() && c2.over_all();
  const Formula& at_end = c1.at_end() && c2.at_end();
  if (&at_start == &c1.at_start() && &over_all == &c1.over_all()
      && &at_end == &c1.at_end()) {
    return c1;
  } else if (&at_start == &c2.at_start() && &over_all == &c2.over_all()
	     && &at_end == &c2.at_end()) {
    return c2;
  } else {
    const Condition& cond = Condition::make(at_start, over_all, at_end);
    Condition::register_use(&c1);
    Condition::unregister_use(&c1);
    Condition::register_use(&c2);
    Condition::unregister_use(&c2);
    return cond;
  }
}


/* Disjunction operator for conditions. */
const Condition& operator||(const Condition& c1, const Condition& c2) {
  const Formula& at_start = c1.at_start() || c2.at_start();
  const Formula& over_all = c1.over_all() || c2.over_all();
  const Formula& at_end = c1.at_end() || c2.at_end();
  if (&at_start == &c1.at_start() && &over_all == &c1.over_all()
      && &at_end == &c1.at_end()) {
    return c1;
  } else if (&at_start == &c2.at_start() && &over_all == &c2.over_all()
	     && &at_end == &c2.at_end()) {
    return c2;
  } else {
    const Condition& cond = Condition::make(at_start, over_all, at_end);
    Condition::register_use(&c1);
    Condition::unregister_use(&c1);
    Condition::register_use(&c2);
    Condition::unregister_use(&c2);
    return cond;
  }
}
