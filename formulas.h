// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Formulas.

#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <set>
#include <vector>

#include "predicates.h"
#include "terms.h"

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

struct Effect;
struct Problem;
struct Domain;
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

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const = 0;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b = NULL) const = 0;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const = 0;

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
  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Constant& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

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
  explicit Constant(bool value);

  friend struct Formula;
};


/* ====================================================================== */
/* Literal */

struct Atom;

/*
 * An abstract literal.
 */
struct Literal : public Formula {
  /* Returns the id for this literal (zero if lifted). */
  size_t id() const { return id_; }

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const = 0;

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const = 0;

  /* Returns the ith term of this literal. */
  virtual const Term& term(size_t i) const = 0;

  /* Returns the atom associated with this literal. */
  virtual const Atom& atom() const = 0;

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Literal& substitution(
      const std::map<Variable, Term>& subst) const = 0;

 protected:
  /* Assigns an id to this literal. */
  void assign_id(bool ground);

private:
  /* Next id for ground literals. */
  static size_t next_id;

  /* Unique id for ground literals (zero if lifted). */
  size_t id_;
};

/*
 * Less than function object for literal pointers.
 */
namespace std {
  template<>
  struct less<const Literal*>
    : public binary_function<const Literal*, const Literal*, bool> {
    /* Comparison function operator. */
    bool operator()(const Literal* l1, const Literal* l2) const {
      return l1->id() < l2->id();
    }
  };
}


/* ====================================================================== */
/* Atom */

/*
 * An atom.
 */
struct Atom : public Literal {
  /* Returns an atomic formula with the given predicate and terms. */
  static const Atom& make(const Predicate& predicate,
                          const std::vector<Term>& terms);

  /* Deletes this atomic formula. */
  virtual ~Atom();

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const { return predicate_; }

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const { return terms_.size(); }

  /* Returns the ith term of this literal. */
  virtual const Term& term(size_t i) const { return terms_[i]; }

  /* Returns the atom associated with this literal. */
  virtual const Atom& atom() const { return *this; }

  /* Returns this formula subject to the given substitutions. */
  virtual const Atom& substitution(const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

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
  std::vector<Term> terms_;

  /* Constructs an atomic formula with the given predicate. */
  explicit Atom(const Predicate& predicate) : predicate_(predicate) {}

  /* Adds a term to this atomic formula. */
  void add_term(const Term& term) { terms_.push_back(term); }
};

/*
 * Less than function object for atom pointers.
 */
namespace std {
  template<>
  struct less<const Atom*> : public less<const Literal*> {
  };
}


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

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const { return atom().predicate(); }

  /* Returns the number of terms of this literal. */
  virtual size_t arity() const { return atom().arity(); }

  /* Returns the ith term of this literal. */
  virtual const Term& term(size_t i) const { return atom().term(i); }

  /* Returns the atom associated with this literal. */
  virtual const Atom& atom() const { return *atom_; }

  /* Returns this formula subject to the given substitutions. */
  virtual const Negation& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

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

/*
 * Less than function object for negation pointers.
 */
namespace std {
  template<>
  struct less<const Negation*> : public less<const Literal*> {
  };
}


/* ====================================================================== */
/* BindingLiteral */

/*
 * A binding literal.
 */
struct BindingLiteral : public Formula {
  /* Returns the variable of this binding literal. */
  const Variable& variable() const { return variable_; }

  /* Returns the step id of the variable, or the given id if no step
     id has been assigned. */
  size_t step_id1(size_t def_id) const { return (id1_ != 0) ? id1_ : def_id; }

  /* Returns the term of this binding literal. */
  const Term& term() const { return term_; }

  /* Returns the step id of the term, or the given id if no step id
     has been assigned. */
  size_t step_id2(size_t def_id) const { return (id2_ != 0) ? id2_ : def_id; }

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

protected:
  /* Constructs a binding literal. */
  BindingLiteral(const Variable& variable, size_t id1,
		 const Term& term, size_t id2)
    : variable_(variable), id1_(id1), term_(term), id2_(id2) {}

private:
  /* Variable of binding literal. */
  Variable variable_;
  /* Step id of variable, or zero if unassigned. */
  size_t id1_;
  /* Term of binding literal. */
  Term term_;
  /* Step id of term, or zero if unassigned. */
  size_t id2_;
};


/* ====================================================================== */
/* Equality */

/*
 * Equality formula.
 */
struct Equality : public BindingLiteral {
  /* Returns an equality of the two terms. */
  static const Formula& make(const Term& term1, const Term& term2);

  /* Returns an equality of the two terms. */
  static const Formula& make(const Term& term1, size_t id1,
			     const Term& term2, size_t id2);

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs an equality with assigned step ids. */
  Equality(const Variable& variable, size_t id1, const Term& term, size_t id2)
    : BindingLiteral(variable, id1, term, id2) {}
};


/* ====================================================================== */
/* Inequality */

/*
 * Inequality formula.
 */
struct Inequality : public BindingLiteral {
  /* Returns an inequality of the two terms. */
  static const Formula& make(const Term& term1, const Term& term2);

  /* Returns an inequality of the two terms. */
  static const Formula& make(const Term& term1, size_t id1,
			     const Term& term2, size_t id2);

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs an inequality with assigned step ids. */
  Inequality(const Variable& variable, size_t id1,
	     const Term& term, size_t id2)
    : BindingLiteral(variable, id1, term, id2) {}
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

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

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

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* The disjuncts. */
  FormulaList disjuncts_;
};


/* ====================================================================== */
/* Quantification */

/*
 * Abstract quantified formula.
 */
struct Quantification : public Formula {
  /* Deletes this quantified formula. */
  virtual ~Quantification();

  /* Adds a quantified variable to this quantified formula. */
  void add_parameter(Variable parameter);

  /* Sets the body of this quantified formula. */
  void set_body(const Formula& body);

  /* Returns the quanitfied variables. */
  const std::vector<Variable>& parameters() const { return parameters_; }

  /* Returns the quantified formula. */
  const Formula& body() const { return *body_; }

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

protected:
  /* Constructs a quantified formula. */
  explicit Quantification(const Formula& body);

 private:
  /* Quanitfied variables. */
  std::vector<Variable> parameters_;
  /* The quantified formula. */
  const Formula* body_;
};


/* ====================================================================== */
/* Exists */

/*
 * Existentially quantified formula.
 */
struct Exists : public Quantification {
  /* Constructs an existentially quantified formula. */
  Exists();

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Quantification& negation() const;
};


/* ====================================================================== */
/* Forall */

/*
 * Universally quantified formula.
 */
struct Forall : public Quantification {
  /* Constructs a universally quantified formula. */
  Forall();

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const Quantification& negation() const;

private:
  /* The cached universal base for this formula. */
  mutable const Formula* universal_base_;
};


/* ====================================================================== */
/* FormulaTime */

/*
 * A formula time.
 */
typedef enum { AT_START, OVER_ALL, AT_END } FormulaTime;


/* ====================================================================== */
/* TimedLiteral */

/*
 * A literal with a time stamp.
 */
struct TimedLiteral : public Formula {
  /* Returns a literal with the given time stamp. */
  static const Formula& make(const Literal& literal, FormulaTime when);

  /* Deletes this timed literal. */
  ~TimedLiteral();

  /* Returns the literal. */
  const Literal& literal() const { return *literal_; }

  /* Returns the time stamp. */
  FormulaTime when() const { return when_; }

  /* Returns a formula that separates the given effect from anything
     definitely asserted by this formula. */
  virtual const Formula& separator(const Effect& effect,
				   const Domain& domain) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const TimedLiteral& substitution(
      const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const std::map<Variable, Term>& subst,
                                       const Problem& problem) const;

  /* Returns the universal base of this formula. */
  virtual const Formula& universal_base(const std::map<Variable, Term>& subst,
                                        const Problem& problem) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(std::ostream& os,
		     size_t step_id, const Bindings& bindings) const;

protected:
  /* Returns the negation of this formula. */
  virtual const TimedLiteral& negation() const;

private:
  /* Literal. */
  const Literal* literal_;
  /* Time stamp. */
  FormulaTime when_;

  /* Constructs a timed literal. */
  TimedLiteral(const Literal& literal, FormulaTime when);
};


#if 0
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
  const Condition& substitution(const std::map<Variable, Term>& subst) const;

  /* Returns an instantiation of this condition. */
  const Condition& instantiation(const std::map<Variable, Term>& subst,
				 const Problem& problem) const;

  /* Returns the heuristic value of this condition. */
  void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
		       const PlanningGraph& pg, size_t step_id,
		       const Bindings* b = NULL) const;

  /* Prints this condition on the given stream with the given bindings. */
  void print(std::ostream& os,
	     size_t step_id, const Bindings& bindings) const;

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
#endif

/* ====================================================================== */
/* AtomSet */

/*
 * A set of atoms.
 */
struct AtomSet : public std::set<const Atom*> {
};


#endif /* FORMULAS_H */
