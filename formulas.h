/* -*-C++-*- */
/*
 * Formulas.
 *
 * $Id: formulas.h,v 1.31 2001-12-25 20:10:56 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <string>
#include <hash_map>
#include <hash_set>
#include "support.h"
#include "heuristics.h"

struct Type;
struct Domain;
struct Problem;
struct Bindings;


struct Term;
struct Variable;

/*
 * Variable substitution.
 */
struct Substitution : public Printable, public gc {
  /* Variable to get substituted. */
  const Variable& var;
  /* Term to substitute with. */
  const Term& term;

  /* Constructs a substitution. */
  Substitution(const Variable& var, const Term& term);

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * List of substitutions.
 */
struct SubstitutionList : public Vector<const Substitution*> {
};

/* Substitution list iterator. */
typedef SubstitutionList::const_iterator SubstListIter;


/*
 * Abstract term.
 */
struct Term
  : public LessThanComparable, public Hashable, public Printable, public gc {
  /* Name of term. */
  const string name;
  /* Type of term. */
  const Type& type;

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
  Variable(const string& name);

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
struct StepVar : public Variable {
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

  /* Checks if this term list is equivalent to the given term list. */
  bool equivalent(const TermList& terms) const;
};

/* Term list iterator. */
typedef TermList::const_iterator TermListIter;


/*
 * List of names.
 */
struct NameList : public Vector<const Name*> {
};

/* Name list iterator. */
typedef NameList::const_iterator NameListIter;


/*
 * Table of names.
 */
struct NameMap : public HashMap<string, const Name*> {
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


/*
 * Abstract formula.
 */
struct Formula : public Hashable, public Printable, public gc {
  /* The true formula. */
  static const Formula& TRUE;
  /* The false formula. */
  static const Formula& FALSE;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const = 0;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const = 0;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const = 0;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const = 0;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas are equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const = 0;

protected:
  /* Returns the hash value of this object. */
  virtual size_t hash_value() const {
    return 0;
  }

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
  FormulaList() {}

  /* Constructs a formula list with a single formula. */
  FormulaList(const Formula* formula) {
    push_back(formula);
  }

  /* Returns an instantiation of this formula list. */
  const FormulaList& instantiation(size_t id) const;

  /* Checks if this formula list is equivalent to the given formula list. */
  bool equivalent(const FormulaList& formulas) const;

  /* Returns the negation of this formula list. */
  const FormulaList& operator!() const;
};

/* A formula list const iterator. */
typedef FormulaList::const_iterator FormulaListIter;


/*
 * An atom.
 */
struct Atom : public Formula {
  /* Predicate of this atomic formula. */
  const string predicate;
  /* Terms of this atomic formula. */
  const TermList& terms;

  /* Constructs an atomic formula. */
  Atom(const string& predicate, const TermList& terms)
    : predicate(predicate), terms(terms) {}

  /* Returns an instantiation of this formula. */
  virtual const Atom& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Atom& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Atom& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * List of atoms.
 */
struct AtomList : public Vector<const Atom*> {
  /* Constructs an empty atom list. */
  AtomList() {}

  /* Constructs an atom list with a single atom. */
  AtomList(const Atom* atom) {
    push_back(atom);
  }

  /* Returns an instantiation of this atom list. */
  const AtomList& instantiation(size_t id) const;

  /* Returns this atom list subject to the given substitutions. */
  const AtomList& substitution(const SubstitutionList& subst) const;
};

typedef AtomList::const_iterator AtomListIter;


/*
 * A negated atom.
 */
struct Negation : public Formula {
  /* The negated atom. */
  const Atom& atom;

  /* Constructs a negated atom. */
  Negation(const Atom& atom)
    : atom(atom) {}

  /* Returns an instantiation of this formula. */
  virtual const Negation& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Negation& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * List of negated atoms.
 */
struct NegationList : public Vector<const Negation*> {
  /* Constructs an empty negation list. */
  NegationList() {}

  /* Constructs a negation list with a single negated atom. */
  NegationList(const Atom* atom) {
    push_back(new Negation(*atom));
  }

  /* Returns an instantiation of this negation list. */
  const NegationList& instantiation(size_t id) const;

  /* Returns this negation list subject to the given substitutions. */
  const NegationList& substitution(const SubstitutionList& subst) const;
};

typedef NegationList::const_iterator NegationListIter;


/*
 * Equality formula.
 * This represents an atomic formula with an equality predicate.
 */
struct Equality : public Formula {
  /* First term of equality. */
  const Term& term1;
  /* Second term of equality. */
  const Term& term2;

  /* Constructs an equality. */
  Equality(const Term& term1, const Term& term2)
    : term1(term1), term2(term2) {}

  /* Returns the instantiation of this formula. */
  virtual const Equality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * Inequality formula.
 * This represents a negated equality formula.
 */
struct Inequality : public Formula {
  /* First term of inequality. */
  const Term& term1;
  /* Second term of inequality. */
  const Term& term2;

  /* Constructs an inequality. */
  Inequality(const Term& term1, const Term& term2)
    : term1(term1), term2(term2) {}

  /* Returns an instantiation of this formula. */
  virtual const Inequality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * Conjunction.
 */
struct Conjunction : public Formula {
  /* The conjuncts. */
  const FormulaList& conjuncts;

  /* Returns an instantiation of this formula. */
  virtual const Conjunction& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a conjunction. */
  Conjunction(const FormulaList& conjuncts)
    : conjuncts(conjuncts) {}

  friend const Formula& operator&&(const Formula& f1, const Formula& f2);
};


/*
 * Disjunction.
 */
struct Disjunction : public Formula {
  /* The disjuncts. */
  const FormulaList& disjuncts;

  /* Returns an instantiation of this formula. */
  virtual const Disjunction& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a disjunction. */
  Disjunction(const FormulaList& disjuncts)
    : disjuncts(disjuncts) {}

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

  /* Returns the heuristic value of this formula. */
  virtual HeuristicValue heuristic_value(const PlanningGraph& pg,
					 const Bindings* b = NULL) const;

protected:
  QuantifiedFormula(const VariableList& parameters, const Formula& body)
    : parameters(parameters), body(body) {}
};


/*
 * Existentially quantified formula.
 */
struct ExistsFormula : public QuantifiedFormula {
  /* Constructs an existentially quantified formula. */
  ExistsFormula(const VariableList& parameters, const Formula& body)
    : QuantifiedFormula(parameters, body) {}

  /* Returns an instantiation of this formula. */
  virtual const ExistsFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * Universally quantified formula.
 */
struct ForallFormula : public QuantifiedFormula {
  /* Constructs a universally quantified formula. */
  ForallFormula(const VariableList& parameters, const Formula& body)
    : QuantifiedFormula(parameters, body) {}

  /* Returns an instantiation of this formula. */
  virtual const ForallFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const Bindings& bindings) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Returns this formula with static predicates assumed true. */
  virtual const Formula& strip_static(const Domain& domain) const;

  /* Returns this formula with equalities/inequalities assumed true. */
  virtual const Formula& strip_equality() const;

  /* Checks if this formula is equivalent to the given formula.  Two
     formulas is equivalent if they only differ in the choice of
     variable names. */
  virtual bool equivalent(const Formula& f) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


#endif /* FORMULAS_H */
