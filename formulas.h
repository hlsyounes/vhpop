/* -*-C++-*- */
/*
 * Formulas.
 *
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
 * $Id: formulas.h,v 6.4 2003-07-21 19:58:14 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <config.h>
#include "predicates.h"
#include "terms.h"
#include "hashing.h"
#include <iostream>
#include <set>
#include <vector>

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

struct Problem;
struct Bindings;
struct HeuristicValue;
struct PlanningGraph;


/* ====================================================================== */
/* Formula */

struct Literal;

/*
 * Abstract formula.
 */
struct Formula {
  /* The true formula. */
  static const Formula& TRUE;
  /* The false formula. */
  static const Formula& FALSE;

  /* Register use of the given formula. */
  static void register_use(const Formula* f) {
    if (f != NULL) {
      f->ref_count_++;
    }
  }

  /* Unregister use of the given formula. */
  static void unregister_use(const Formula* f) {
    if (f != NULL) {
      f->ref_count_--;
      if (f->ref_count_ == 0) {
	delete f;
      }
    }
  }

  /* Deletes this formula. */
  virtual ~Formula();

  /* Tests if this formula is a tautology. */
  bool tautology() const { return this == &TRUE; }

  /* Tests if this formula is a contradiction. */
  bool contradiction() const { return this == &FALSE; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const = 0;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b = NULL) const = 0;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const = 0;

protected:
  /* Constructs a formula. */
  Formula();

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

  friend const Formula& operator!(const Formula& f);
};

/* Negation operator for formulas. */
const Formula& operator!(const Formula& f);

/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2);

/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2);


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
struct FormulaList : public std::vector<const Formula*> {
};


/* ====================================================================== */
/* Constant */

/*
 * A formula with a constant truth value.
 */
struct Constant : public Formula {
  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Constant& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns a negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constant representing true. */
  static const Constant TRUE_;
  /* Constant representing false. */
  static const Constant FALSE_;

  /* Value of this constant. */
  bool value_;

  /* Constructs a constant formula. */
  Constant(bool value);

  friend struct Formula;
};


/* ====================================================================== */
/* Literal */

/*
 * An abstract literal.
 */
struct Literal : public Formula {
  /* Returns the predicate of this literal. */
  virtual Predicate predicate() const = 0;

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const = 0;

  /* Returns the ith term of this literal. */
  virtual Term term(size_t i) const = 0;

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;
};


/* ====================================================================== */
/* Atom */

/*
 * An atom.
 */
struct Atom : public Literal {
  /* Returns an atomic formula with the given predicate and terms. */
  static const Atom& make(Predicate predicate, const TermList& terms);

  /* Deletes this atomic formula. */
  virtual ~Atom();

  /* Returns the predicate of this literal. */
  virtual Predicate predicate() const { return predicate_; }

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const { return terms_.size(); }

  /* Returns the ith term of this literal. */
  virtual Term term(size_t i) const { return terms_[i]; }

  /* Returns this formula subject to the given substitutions. */
  virtual const Atom& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;

private:
  /* Less-than comparison function object for atoms. */
  struct AtomLess
    : public std::binary_function<const Atom*, const Atom*, bool> {
    /* Comparison function. */
    bool operator()(const Atom* a1, const Atom* a2) const;
  };

  /* A table of atomic formulas. */
  struct AtomTable : std::set<const Atom*, AtomLess> {
  };

  /* Table of atomic formulas. */
  static AtomTable atoms;

  /* Predicate of this atom. */
  Predicate predicate_;
  /* Terms of this atom. */
  TermList terms_;

  /* Constructs an atomic formula with the given predicate. */
  Atom(Predicate predicate);

  /* Adds a term to this atomic formula. */
  void add_term(Term term) { terms_.push_back(term); }
};


/* ====================================================================== */
/* Negation */

/*
 * A negated atom.
 */
struct Negation : public Literal {
  /* Returns a negation of the given atom. */
  static const Negation& make(const Atom& atom);

  /* Deletes this negated atom. */
  virtual ~Negation();

  /* Returns the the negated atom. */
  const Atom& atom() const { return *atom_; }

  /* Returns the predicate of this literal. */
  virtual Predicate predicate() const { return atom().predicate(); }

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const { return atom().arity(); }

  /* Returns the ith term of this literal. */
  virtual Term term(size_t i) const { return atom().term(i); }

  /* Returns this formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;

private:
  /* Less-than comparison function object for negated atoms. */
  struct NegationLess
    : public std::binary_function<const Negation*, const Negation*, bool> {
    /* Comparison function. */
    bool operator()(const Negation* n1, const Negation* n2) const;
  };

  /* A table of negated atoms. */
  struct NegationTable : std::set<const Negation*, NegationLess> {
  };

  /* Table of negated atoms. */
  static NegationTable negations;

  /* The negated atom. */
  const Atom* atom_;

  /* Constructs a negated atom. */
  explicit Negation(const Atom& atom);
};


/* ====================================================================== */
/* BindingLiteral */

/*
 * A binding literal.
 */
struct BindingLiteral : public Formula {
  /* Returns the first term of binding literal. */
  Term term1() const { return term1_; }

  /* Returns the step id of the first term, or the given id if no step
     id has been assigned. */
  size_t step_id1(size_t def_id) const { return (id1_ != 0) ? id1_ : def_id; }

  /* Returns the second term of binding literal. */
  Term term2() const { return term2_; }

  /* Returns the step id of the second term, or the given id if no
     step id has been assigned. */
  size_t step_id2(size_t def_id) const { return (id2_ != 0) ? id2_ : def_id; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;

protected:
  /* Constructs a binding literal. */
  BindingLiteral(Term term1, size_t id1, Term term2, size_t id2);

private:
  /* First term of binding literal. */
  Term term1_;
  /* Step id of first term, or zero if unassigned. */
  size_t id1_;
  /* Second term of binding literal. */
  Term term2_;
  /* Step id of second term, or zero if unassigned. */
  size_t id2_;
};


/* ====================================================================== */
/* Equality */

/*
 * Equality formula.
 */
struct Equality : public BindingLiteral {
  /* Returns an equality of the two terms. */
  static const Formula& make(Term term1, Term term2);

  /* Returns an equality of the two terms. */
  static const Formula& make(Term term1, size_t id1, Term term2, size_t id2);

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs an equality with assigned step ids. */
  Equality(Term term1, size_t id1, Term term2, size_t id2);
};


/* ====================================================================== */
/* Inequality */

/*
 * Inequality formula.
 */
struct Inequality : public BindingLiteral {
  /* Returns an inequality of the two terms. */
  static const Formula& make(Term term1, Term term2);

  /* Returns an inequality of the two terms. */
  static const Formula& make(Term term1, size_t id1, Term term2, size_t id2);

  /* Constructs an inequality with assigned step ids. */
  Inequality(Term term1, size_t id1, Term term2, size_t id2);

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/* ====================================================================== */
/* Conjunction */

/*
 * Conjunction.
 */
struct Conjunction : public Formula {
  /* Constructs an empty conjunction. */
  Conjunction();

  /* Deletes this conjunction. */
  virtual ~Conjunction();

  /* Adds a conjunct to this conjunction. */
  void add_conjunct(const Formula& conjunct);

  /* Returns the conjuncts. */
  const FormulaList& conjuncts() const { return conjuncts_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* The conjuncts. */
  FormulaList conjuncts_;
};


/* ====================================================================== */
/* Disjunction */

/*
 * Disjunction.
 */
struct Disjunction : public Formula {
  /* Constructs an empty disjunction. */
  Disjunction();

  /* Deletes this disjunction. */
  virtual ~Disjunction();

  /* Adds a disjunct to this disjunction. */
  void add_disjunct(const Formula& disjunct);

  /* Returns the disjuncts. */
  const FormulaList& disjuncts() const { return disjuncts_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* QuantifiedFormula */

/*
 * Abstract quantified formula.
 */
struct QuantifiedFormula : public Formula {
  /* Deletes this quantified formula. */
  virtual ~QuantifiedFormula();

  /* Adds a quantified variable to this quantified formula. */
  void add_parameter(Variable parameter);

  /* Sets the body of this quantified formula. */
  void set_body(const Formula& body);

  /* Returns the quanitfied variables. */
  const VariableList& parameters() const { return parameters_; }

  /* Returns the quantified formula. */
  const Formula& body() const { return *body_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Literal& literal) const;

protected:
  /* Constructs a quantified formula. */
  explicit QuantifiedFormula(const Formula& body);

private:
  /* Quanitfied variables. */
  VariableList parameters_;
  /* The quantified formula. */
  const Formula* body_;
};


/* ====================================================================== */
/* Exists */

/*
 * Existentially quantified formula.
 */
struct Exists : public QuantifiedFormula {
  /* Constructs an existentially quantified formula. */
  Exists();

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/* ====================================================================== */
/* Forall */

/*
 * Universally quantified formula.
 */
struct Forall : public QuantifiedFormula {
  /* Constructs a universally quantified formula. */
  Forall();

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionMap& subst,
				       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const SubstitutionMap& subst,
					const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os, const PredicateTable& predicates,
		     const TermTable& terms, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/* ====================================================================== */
/* FormulaTime */

/*
 * A formula time.
 */
typedef enum { AT_START, OVER_ALL, AT_END } FormulaTime;


/* ====================================================================== */
/* Condition */

/*
 * An action or effect condition.
 */
struct Condition {
  /* The true condition. */
  static const Condition TRUE;
  /* The false condition. */
  static const Condition FALSE;

  /* Register use of the given condition. */
  static void register_use(const Condition* c) {
    if (c != NULL) {
      c->ref_count_++;
    }
  }

  /* Unregister use of the given condition. */
  static void unregister_use(const Condition* c) {
    if (c != NULL) {
      c->ref_count_--;
      if (c->ref_count_ == 0) {
	delete c;
      }
    }
  }

  /* Returns a condition. */
  static const Condition& make(const Formula& at_start,
			       const Formula& over_all,
			       const Formula& at_end);

  /* Returns a condition. */
  static const Condition& make(const Formula& formula, FormulaTime when);

  /* Deletes this condition. */
  ~Condition();

  /* Returns the start condition. */
  const Formula& at_start() const { return *at_start_; }

  /* Returns the interval condition. */
  const Formula& over_all() const { return *over_all_; }

  /* Returns the end condition. */
  const Formula& at_end() const { return *at_end_; }

  /* Tests if this condition is a tautology. */
  bool tautology() const { return this == &TRUE; }

  /* Tests if this condition is a contradiction. */
  bool contradiction() const { return this == &FALSE; }

  /* Returns this condition subject to the given substitutions. */
  const Condition& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this condition. */
  const Condition& instantiation(const SubstitutionMap& subst,
				 const Problem& problem) const;

  /* Returns the universal base of this condition. */
  const Condition& universal_base(const SubstitutionMap& subst,
				  const Problem& problem) const;

  /* Returns the heuristic value of this condition. */
  void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
		       const PlanningGraph& pg, size_t step_id,
		       const Bindings* b = NULL) const;

  /* Prints this condition on the given stream with the given bindings. */
  void print(std::ostream& os, const PredicateTable& predicates,
	     const TermTable& terms, size_t step_id,
	     const Bindings& bindings) const;

private:
  /* Start condition. */
  const Formula* at_start_;
  /* Interval condition. */
  const Formula* over_all_;
  /* End condition. */
  const Formula* at_end_;
  /* Reference counter. */
  mutable size_t ref_count_;

  /* Constructs a Boolean condition. */
  Condition(bool b);

  /* Constructs a condition. */
  Condition(const Formula& at_start, const Formula& over_all,
	    const Formula& at_end);
};

/* Negation operator for conditions. */
const Condition& operator!(const Condition& c);

/* Conjunction operator for conditions. */
const Condition& operator&&(const Condition& c1, const Condition& c2);

/* Disjunction operator for conditions. */
const Condition& operator||(const Condition& c1, const Condition& c2);


/* ====================================================================== */
/* AtomList */

/*
 * List of atoms.
 */
struct AtomList : public std::vector<const Atom*> {
};

/* Atom list iterator. */
typedef AtomList::const_iterator AtomListIter;


/* ====================================================================== */
/* NegationList */

/*
 * List of negated atoms.
 */
struct NegationList : public std::vector<const Negation*> {
};

/* Negation list iterator */
typedef NegationList::const_iterator NegationListIter;


#endif /* FORMULAS_H */
