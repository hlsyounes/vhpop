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
 * $Id: bindings.h,v 4.1 2002-09-20 16:44:39 lorens Exp $
 */
#ifndef BINDINGS_H
#define BINDINGS_H

#include <set>
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
 * Variable binding.
 */
struct Binding {
  /* Constructs a variable binding from the given substitution. */
  Binding(const Substitution& s, bool equality, const Reason& reason);

  /* Constructs a variable binding. */
  Binding(const Variable& var, size_t var_id, const Term& term, size_t term_id,
	  bool equality, const Reason& reason);

  /* Returns the variable of this binding. */
  const Variable& var() const { return *var_; }

  /* Returns the step id for the variable. */
  size_t var_id() const { return var_id_; }

  /* Returns the term of this binding. */
  const Term& term() const { return *term_; }

  /* Return the step id for the term. */
  size_t term_id() const { return term_id_; }

  /* Checks if this is an equality binding. */
  bool equality() const { return equality_; }

  /* Returns the reason for this binding. */
  const Reason& reason() const;

private:
  /* A variable. */
  const Variable* var_;
  /* Step id for the variable. */
  size_t var_id_;
  /* A term either bound to, or separated from the variable. */
  const Term* term_;
  /* Step id for the term. */
  size_t term_id_;
  /* Whether or not this is an equality binding. */
  bool equality_;
#ifdef TRANSFORMATIONAL
  /* Reason for this binding. */
  const Reason* reason_;
#endif

  friend ostream& operator<<(ostream& os, const Binding& b);
};

/* Output operator for variable bindings. */
ostream& operator<<(ostream& os, const Binding& b);


/* ====================================================================== */
/* BindingList */

/*
 * List of bindings.
 */
struct BindingList : vector<Binding> {
};

/* Iterator for binding lists. */
typedef BindingList::const_iterator BindingListIter;


/* ====================================================================== */
/* BindingChain */

/*
 * Chain of bindings.
 */
typedef CollectibleChain<Binding> BindingChain;


/* ====================================================================== */
/* NameSet */

/*
 * A set of names.
 */
struct NameSet : public set<const Name*, less<const LessThanComparable*> > {
};

/* Iterator for name sets. */
typedef NameSet::const_iterator NameSetIter;


/* ====================================================================== */
/* ActionDomain */

/*
 * Domain for action parameters.
 */
struct ActionDomain {
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

private:
  /* A list of parameter tuples. */
  struct TupleList : public vector<const NameList*> {
  };

  /* A tuple list iterator. */
  typedef TupleList::const_iterator TupleListIter;

  /* Possible parameter tuples. */
  TupleList tuples_;
  /* Projections. */
  vector<NameSet*> projections_;

  friend ostream& operator<<(ostream& os, const ActionDomain& ad);
};

/* Output operator for action domains. */
ostream& operator<<(ostream& os, const ActionDomain& ad);


/* ====================================================================== */
/* VarsetChain */

struct Varset;

/*
 * Chain of varsets.
 */
typedef CollectibleChain<Varset> VarsetChain;


/* ====================================================================== */
/* StepDomainChain */

struct StepDomain;

/*
 * Chain of step domains.
 */
typedef CollectibleChain<StepDomain> StepDomainChain;


/* ====================================================================== */
/* Bindings */

/*
 * A collection of variable bindings.
 */
struct Bindings : public Printable, public Collectible {
  /* Creates a collection of variable bindings with the given equality
     and inequality bindings. Parameter constrains are used if pg is
     not NULL. */
  static const Bindings* make_bindings(const CollectibleChain<Step>* steps,
				       const PlanningGraph* pg,
				       const BindingChain* equalities,
				       const BindingChain* inequalities);

  /* Checks if the given formulas can be unified. */
  static bool unifiable(const Literal& l1, size_t id1,
			const Literal& l2, size_t id2);

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  static bool unifiable(SubstitutionList& mgu,
			const Literal& l1, size_t id1,
			const Literal& l2, size_t id2);

  /* Deletes this binding collection. */
  virtual ~Bindings();

  /* Returns the equality bindings. */
  const BindingChain* equalities() const;

  /* Return the inequality bindings. */
  const BindingChain* inequalities() const;

  /* Returns the binding for the given term, or the term itself if it
     is not bound to a single name. */
  const Term& binding(const Term& t, size_t step_id) const;

  /* Returns the domain for the given step variable. */
  const NameSet* domain(const Variable& v, size_t step_id) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified. */
  bool affects(const Literal& l1, size_t id1,
	       const Literal& l2, size_t id2) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified; the most general unifier
     is added to the provided substitution list. */
  bool affects(SubstitutionList& mgu,
	       const Literal& l1, size_t id1,
	       const Literal& l2, size_t id2) const;

  /* Checks if the given formulas can be unified. */
  bool unify(const Literal& l1, size_t id1,
	     const Literal& l2, size_t id2) const;

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  bool unify(SubstitutionList& mgu,
	     const Literal& l1, size_t id1,
	     const Literal& l2, size_t id2) const;

  /* Checks if the given equality is consistent with the current
     bindings. */
  bool consistent_with(const Equality& eq, size_t step_id) const;

  /* Checks if the given inequality is consistent with the current
     bindings. */
  bool consistent_with(const Inequality& neq, size_t step_id) const;

  /* Returns the binding collection obtained by adding the given
     bindings to this binding collection, or NULL if the new bindings
     are inconsistent with the current. */
  const Bindings* add(const BindingList& new_bindings,
		      bool test_only = false) const;

  /* Returns the binding collection obtained by adding the constraints
     associated with the given step to this binding collection, or
     NULL if the new binding collection would be inconsistent. */
  const Bindings* add(size_t step_id, const Action& step_action,
		      const PlanningGraph& pg, bool test_only = false) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Varsets representing the transitive closure of the bindings. */
  const VarsetChain* varsets_;
  /* Highest step id of variable in varsets. */
  size_t high_step_;
  /* Step domains. */
  const StepDomainChain* step_domains_;
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
