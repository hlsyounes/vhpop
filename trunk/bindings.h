/* -*-C++-*- */
/*
 * Binding constraints.
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
 * $Id: bindings.h,v 3.1 2002-03-18 09:23:24 lorens Exp $
 */
#ifndef BINDINGS_H
#define BINDINGS_H

#include "support.h"
#include "chain.h"


struct Term;
struct Name;
struct Variable;
struct Substitution;
struct SubstitutionList;
struct NameList;
struct Literal;
struct Equality;
struct Inequality;
struct Action;
struct Reason;
struct Step;
struct PlanningGraph;


/* ====================================================================== */
/* Binding */

/*
 * Abstract variable binding.
 */
struct Binding : public Printable, public gc {
  /* Returns the variable of this binding. */
  const Variable& var() const { return *var_; }

  /* Returns the term of this binding. */
  const Term& term() const { return *term_; }

  /* Returns the reason for this binding. */
  const Reason& reason() const;

protected:
  /* Constructs an abstract variable binding. */
  Binding(const Variable& var, const Term& term, const Reason& reason);

private:
  /* A variable. */
  const Variable* var_;
  /* A term either bound to, or separated from the variable. */
  const Term* term_;
#ifdef TRANSFORMATIONAL
  /* Reason for this binding. */
  const Reason* reason_;
#endif
};


/* ====================================================================== */
/* EqualityBinding */

/*
 * Equality binding.
 */
struct EqualityBinding : public Binding {
  /* Constructs an equality binding from the given substitution. */
  EqualityBinding(const Substitution& s, const Reason& reason);

  /* Construct an equality binding, binding the given variable to the
     given term. */
  EqualityBinding(const Variable& var, const Term& term,
		  const Reason& reason);

protected:
  /* Prints this equality binding on the given stream. */
  virtual void print(ostream& os) const;
};


/* ====================================================================== */
/* InequalityBinding */

/*
 * Inequality binding.
 */
struct InequalityBinding : public Binding {
  /* Constructs an inequality binding from the given substitution. */
  InequalityBinding(const Substitution& s, const Reason& reason);

  /* Constructs an inequality binding, separating the given variable
     from the given term. */
  InequalityBinding(const Variable& var, const Term& term,
		    const Reason& reason);

protected:
  /* Prints this inequality binding on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * List of bindings.
 */
struct BindingList : Vector<const Binding*> {
};

/* Iterator for binding lists. */
typedef BindingList::const_iterator BindingListIter;


/*
 * Chain of bindings.
 */
typedef Chain<const Binding*> BindingChain;


/*
 * A set of names.
 */
struct NameSet : public Set<const Name*, less<const LessThanComparable*> > {
};

/* Iterator for name sets. */
typedef NameSet::const_iterator NameSetIter;


/*
 * A set of variables.
 */
struct VariableSet
  : public Set<const Variable*, less<const LessThanComparable*> > {
  static const VariableSet& EMPTY;
};

/* Iterator for variable sets. */
typedef VariableSet::const_iterator VariableSetIter;


/*
 * Domain for action parameters.
 */
struct ActionDomain : public Printable, public gc {
  /* Constructs an action domain with a single tuple. */
  ActionDomain(const NameList& tuple);

  /* Number of tuples. */
  size_t size() const;

  /* Adds a tuple to this domain. */
  void add(const NameList& tuple);

  /* Returns the set of names from the given column. */
  const NameSet& projection(size_t column) const;

  /* Returns the size of the projection of the given column. */
  const size_t projection_size(size_t column) const;

  /* Returns a domain where the given column has been restricted to
     the given name, or NULL if this would leave an empty domain. */
  const ActionDomain* restrict(const Name& name, size_t column) const;

  /* Returns a domain where the given column has been restricted to
     the given set of names, or NULL if this would leave an empty
     domain. */
  const ActionDomain* restrict(const NameSet& names, size_t column) const;

  /* Returns a domain where the given column exclues the given name,
     or NULL if this would leave an empty domain. */
  const ActionDomain* exclude(const Name& name, size_t column) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* A list of parameter tuples. */
  struct TupleList : public Vector<const NameList*> {
  };

  /* A tuple list iterator. */
  typedef TupleList::const_iterator TupleListIter;

  /* Possible parameter tuples. */
  TupleList tuples;
  /* Projections. */
  Vector<NameSet*> projections;
};


struct Varset;

/*
 * Chain of varsets.
 */
typedef Chain<const Varset*> VarsetChain;

struct StepDomain;

/*
 * Chain of step domains.
 */
typedef Chain<const StepDomain*> StepDomainChain;


/* ====================================================================== */
/* Bindings */

/*
 * A collection of variable bindings.
 */
struct Bindings : public Printable, public gc {
  /* Creates a binding collection with parameter constrains if pg is
     not NULL, or an empty binding collection otherwise. */
  static const Bindings& make_bindings(const Chain<const Step*>* steps,
				       const PlanningGraph* pg);

  /* Creates a collection of variable bindings with the given equality
     and inequality bindings. */
  static const Bindings* make_bindings(const Chain<const Step*>* steps,
				       const PlanningGraph* pg,
				       const BindingChain* equalities,
				       const BindingChain* inequalities);

  /* Checks if the given formulas can be unified. */
  static bool unifiable(const Literal& l1, const Literal& l2);

  /* Returns the equality bindings. */
  const BindingChain* equalities() const;

  /* Return the inequality bindings. */
  const BindingChain* inequalities() const;

  /* Returns the binding for the given term, or the term itself if it
     is not bound to a single name. */
  const Term& binding(const Term& t) const;

  /* Returns the domain for the given step variable. */
  const NameSet* domain(const Variable& v) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified. */
  bool affects(const Literal& l1, const Literal& l2) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified; the most general unifier
     is added to the provided substitution list. */
  bool affects(SubstitutionList& mgu,
	       const Literal& l1, const Literal& l2) const;

  /* Checks if the given formulas can be unified. */
  bool unify(const Literal& l1, const Literal& l2) const;

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  bool unify(SubstitutionList& mgu,
	     const Literal& l1, const Literal& l2) const;

  /* Checks if the given equality is consistent with the current
     bindings. */
  bool consistent_with(const Equality& eq) const;

  /* Checks if the given inequality is consistent with the current
     bindings. */
  bool consistent_with(const Inequality& neq) const;

  /* Returns the binding collection obtained by adding the given
     bindings to this binding collection, or NULL if the new bindings
     are inconsistent with the current. */
  const Bindings* add(const BindingList& new_bindings) const;

  /* Returns the binding collection obtained by adding the constraints
     associated with the given step to this binding collection, or
     NULL if the new binding collection would be inconsistent. */
  const Bindings* add(size_t step_id, const Action* step_action,
		      const PlanningGraph& pg) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Varsets representing the transitive closure of the bindings. */
  const VarsetChain* const varsets_;
  /* Highest step id of variable in varsets. */
  const size_t high_step_;
  /* Step domains. */
  const StepDomainChain* const step_domains_;
#ifdef TRANSFORMATIONAL
  /* Equality bindings. */
  const BindingChain* equalities_;
  /* Inequality bindings. */
  const BindingChain* inequalities_;
#endif

  /* Constructs a binding collection. */
  Bindings(const VarsetChain* varsets, size_t high_step,
	   const StepDomainChain* step_domains,
	   const BindingChain* equalities, const BindingChain* inequalities);
};

#endif /* BINDINGS_H */
