/* -*-C++-*- */
/*
 * Formulas.
 *
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
 * $Id: formulas.h,v 3.6 2002-03-15 19:01:48 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include "support.h"

struct Type;
struct Domain;
struct Problem;
struct Bindings;
struct HeuristicValue;
struct PlanningGraph;


struct Term;
struct Variable;


/* ====================================================================== */
/* Substitution */

/*
 * Variable substitution.
 */
struct Substitution : public Printable {
  /* Constructs a substitution. */
  Substitution(const Variable& var, const Term& term);

  /* Returns the variable to get substituted. */
  const Variable& var() const { return *var_; }

  /* Returns the term to substitute with. */
  const Term& term() const { return *term_; }

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Variable to get substituted. */
  const Variable* var_;
  /* Term to substitute with. */
  const Term* term_;
};


/* ====================================================================== */
/* SubstitutionList */

/*
 * List of substitutions.
 */
struct SubstitutionList : public vector<Substitution> {
};

/* Substitution list iterator. */
typedef SubstitutionList::const_iterator SubstListIter;


/* ====================================================================== */
/* Term */

/*
 * Abstract term.
 */
struct Term : public LessThanComparable, public Hashable, public Printable {
  /* Deletes this term. */
  virtual ~Term();

  /* Returns the name of this term. */
  const string& name() const { return name_; }

  /* Returns the type of this term. */
  const Type& type() const { return *type_; }

  /* Returns an instantiation of this term. */
  virtual const Term& instantiation(size_t id) const = 0;

  /* Returns an instantiation of this term. */
  const Term& instantiation(const Bindings& bindings) const;

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const = 0;

  /* Checks if this term is equivalent to the given term.  Two terms
     are equivalent if they are the same name, or if they are both
     variables. */
  virtual bool equivalent(const Term& t) const = 0;

protected:
  /* Constructs an abstract term with the given name. */
  Term(const string& name, const Type& type);

  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Name of term. */
  string name_;
  /* Type of term. */
  const Type* type_;
};


/*
 * Name.
 */
struct Name : public Term {
  /* Constructs a name. */
  Name(const string& name, const Type& type);

  /* Returns an instantiation of this term. */
  virtual const Name& instantiation(size_t id) const;

  /* Returns this term subject to the given substitutions. */
  virtual const Name& substitution(const SubstitutionList& subst) const;

  /* Checks if this term is equivalent to the given term.  Two terms
     are equivalent if they are the same name, or if they are both
     variables. */
  virtual bool equivalent(const Term& t) const;
};


/*
 * Variable.
 */
struct Variable : public Term {
  /* Constructs a variable with the given name. */
  explicit Variable(const string& name);

  /* Constructs a variable with the given name and type. */
  Variable(const string& name, const Type& type);

  /* Returns an instantiation of this term. */
  virtual const Variable& instantiation(size_t id) const;

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const;

  /* Checks if this term is equivalent to the given term.  Two terms
     are equivalent if they are the same name, or if they are both
     variables. */
  virtual bool equivalent(const Term& t) const;
};


/*
 * Instantiated variable.
 */
struct StepVar : public Variable, public gc {
  /* The id of the step that this variable belongs to. */
  const size_t id;

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constructs an instantiated variable. */
  StepVar(const Variable& var, size_t id);

  friend const Variable& Variable::instantiation(size_t id) const;
};


/*
 * List of terms.
 */
struct TermList : public Vector<const Term*> {
  /* Returns an instantiation of this term list. */
  const TermList& instantiation(size_t id) const;

  /* Returns an instantiation of this term list. */
  const TermList& instantiation(const Bindings& bindings) const;

  /* Returns this term list subject to the given substitutions. */
  const TermList& substitution(const SubstitutionList& subst) const;
};

/* Term list iterator. */
typedef TermList::const_iterator TermListIter;


/*
 * List of names.
 */
struct NameList : public vector<const Name*> {
};

/* Name list iterator. */
typedef NameList::const_iterator NameListIter;


/*
 * Table of names.
 */
struct NameMap : public hash_map<string, const Name*> {
};

/* Name table iterator. */
typedef NameMap::const_iterator NameMapIter;


/*
 * List of variables.
 */
struct VariableList : public Vector<const Variable*> {
  /* An empty variable list. */
  static const VariableList& EMPTY;

  /* Returns an instantiation of this variable list. */
  const VariableList& instantiation(size_t id) const;
};

/* Variable list iterator. */
typedef VariableList::const_iterator VarListIter;


struct Literal;

/*
 * Abstract formula.
 */
struct Formula : public Printable, public gc {
  /* Possible temporal annotations for formulas. */
  typedef enum { AT_START, AT_END, OVER_ALL } FormulaTime;

  /* The true formula. */
  static const Formula& TRUE;
  /* The false formula. */
  static const Formula& FALSE;

  /* Checks if this formula is a tautology. */
  bool tautology() const;

  /* Checks if this formula is a contradiction. */
  bool contradiction() const;

  /* Checks if this formula is either a tautology or contradiction. */
  bool constant() const;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const = 0;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b = NULL) const = 0;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const = 0;

protected:
  /* Returns the negation of this formula. */
  virtual const Formula& negation() const = 0;

  friend const Formula& operator!(const Formula& f);
};

/* Negation operator for formulas. */
inline const Formula& operator!(const Formula& f) {
  return f.negation();
}

/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2);

/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2);


/*
 * List of formulas.
 */
struct FormulaList : public Vector<const Formula*> {
  /* Constructs an empty formula list. */
  FormulaList();

  /* Constructs a formula list with a single formula. */
  explicit FormulaList(const Formula* formula);

  /* Returns an instantiation of this formula list. */
  const FormulaList& instantiation(size_t id) const;

  /* Returns the negation of this formula list. */
  const FormulaList& negation() const;
};

/* A formula list const iterator. */
typedef FormulaList::const_iterator FormulaListIter;


/*
 * A formula with a constant truth value.
 */
struct Constant : public Formula {
  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(const SubstitutionList& subst,
					const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Constant& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Constant& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Value of this constant. */
  bool value_;

  /* Constructs a constant formula. */
  Constant(bool value);

  friend struct Formula;
};


/*
 * An abstract literal.
 */
struct Literal : public Formula {
  /* Temporal annotation for this formula. */
  const FormulaTime when;

  /* Returns the predicate of this literal. */
  virtual const string& predicate() const = 0;

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const = 0;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

protected:
  /* Constructs a literal. */
  explicit Literal(FormulaTime when);

  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const = 0;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const = 0;

  friend bool operator==(const Literal& l1, const Literal& l2);
  friend struct hash<Literal>;
  friend struct hash<const Literal*>;
};

/* Equality operator for literals. */
inline bool operator==(const Literal& l1, const Literal& l2) {
  return l1.equals(l2);
}

/* Inequality operator for literals. */
inline bool operator!=(const Literal& l1, const Literal& l2) {
  return !(l1 == l2);
}

/*
 * Equality function object for literal pointers.
 */
struct equal_to<const Literal*>
  : public binary_function<const Literal*, const Literal*, bool> {
  bool operator()(const Literal* l1, const Literal* l2) const {
    return *l1 == *l2;
  }
};

/*
 * Hash function object for literals.
 */
struct hash<Literal> {
  size_t operator()(const Literal& l) const {
    return l.hash_value();
  }
};

/*
 * Hash function object for literal pointers.
 */
struct hash<const Literal*> {
  size_t operator()(const Literal* l) const {
    return l->hash_value();
  }
};


/*
 * An atom.
 */
struct Atom : public Literal {
  /* Constructs an atomic formula. */
  Atom(const string& predicate, const TermList& terms, FormulaTime when);

  /* Returns the predicate of this literal. */
  virtual const string& predicate() const;

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const;

  /* Returns an instantiation of this formula. */
  virtual const Atom& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Atom& substitution(const SubstitutionList& subst) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;

private:
  /* Predicate of this atom. */
  string predicate_;
  /* Terms of this atom. */
  const TermList& terms_;
};


/*
 * A negated atom.
 */
struct Negation : public Literal {
  /* The negated atom. */
  const Atom& atom;

  /* Constructs a negated atom. */
  explicit Negation(const Atom& atom);

  /* Returns the predicate of this literal. */
  virtual const string& predicate() const;

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const;

  /* Returns an instantiation of this formula. */
  virtual const Negation& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionList& subst) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;
};


/*
 * A binding literal.
 */
struct BindingLiteral : public Formula {
  /* First term of binding literal. */
  const Term& term1;
  /* Second term of binding literal. */
  const Term& term2;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

protected:
  /* Constructs a binding literal. */
  BindingLiteral(const Term& term1, const Term& term2);
};


/*
 * Equality formula.
 */
struct Equality : public BindingLiteral {
  /* Constructs an equality. */
  Equality(const Term& term1, const Term& term2);

  /* Returns the instantiation of this formula. */
  virtual const Equality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const BindingLiteral& negation() const;
};


/*
 * Inequality formula.
 */
struct Inequality : public BindingLiteral {
  /* Constructs an inequality. */
  Inequality(const Term& term1, const Term& term2);

  /* Returns an instantiation of this formula. */
  virtual const Inequality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const BindingLiteral& negation() const;
};


/*
 * Conjunction.
 */
struct Conjunction : public Formula {
  /* The conjuncts. */
  const FormulaList& conjuncts;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Conjunction& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a conjunction. */
  Conjunction(const FormulaList& conjuncts);

  friend const Formula& operator&&(const Formula& f1, const Formula& f2);
};


/*
 * Disjunction.
 */
struct Disjunction : public Formula {
  /* The disjuncts. */
  const FormulaList& disjuncts;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Disjunction& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a disjunction. */
  Disjunction(const FormulaList& disjuncts);

  friend const Formula& operator||(const Formula& f1, const Formula& f2);
};


/*
 * Abstract quantified formula.
 */
struct QuantifiedFormula : public Formula {
  /* Quanitfied variables. */
  const VariableList& parameters;
  /* The quantified formula. */
  const Formula& body;

  /* Checks if this formula asserts the given atom. */
  virtual bool asserts(const Literal& literal) const;

protected:
  /* Constructs a quantified formula. */
  QuantifiedFormula(const VariableList& parameters, const Formula& body);
};


/*
 * Existentially quantified formula.
 */
struct ExistsFormula : public QuantifiedFormula {
  /* Constructs an existentially quantified formula. */
  ExistsFormula(const VariableList& parameters, const Formula& body);

  /* Returns an instantiation of this formula. */
  virtual const ExistsFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/*
 * Universally quantified formula.
 */
struct ForallFormula : public QuantifiedFormula {
  /* Constructs a universally quantified formula. */
  ForallFormula(const VariableList& parameters, const Formula& body);

  /* Returns an instantiation of this formula. */
  virtual const ForallFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static literals assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b = NULL) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg,
			       const Bindings* b) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/*
 * List of atoms.
 */
struct AtomList : public Vector<const Atom*> {
  /* Constructs an empty atom list. */
  AtomList();

  /* Constructs an atom list with a single atom. */
  explicit AtomList(const Atom* atom);

  /* Returns an instantiation of this atom list. */
  const AtomList& instantiation(size_t id) const;

  /* Returns this atom list subject to the given substitutions. */
  const AtomList& substitution(const SubstitutionList& subst) const;
};

/* Atom list iterator. */
typedef AtomList::const_iterator AtomListIter;


/*
 * List of negated atoms.
 */
struct NegationList : public Vector<const Negation*> {
  /* Constructs an empty negation list. */
  NegationList();

  /* Constructs a negation list with a single negated atom. */
  NegationList(const Atom* atom);

  /* Returns an instantiation of this negation list. */
  const NegationList& instantiation(size_t id) const;

  /* Returns this negation list subject to the given substitutions. */
  const NegationList& substitution(const SubstitutionList& subst) const;
};

/* Negation list iterator */
typedef NegationList::const_iterator NegationListIter;


#endif /* FORMULAS_H */
