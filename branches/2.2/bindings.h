/* -*-C++-*- */
/*
 * Binding constraints.
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
 * $Id: bindings.h,v 4.6 2003-03-01 18:53:22 lorens Exp $
 */
#ifndef BINDINGS_H
#define BINDINGS_H

#include <config.h>
#include "formulas.h"
#include "chain.h"
#include "hashing.h"
#include <set>

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
  Binding(const Substitution& s, bool equality, const Reason& reason)
    : var_(&s.var()), var_id_(s.var_id()),
      term_(&s.term()), term_id_(s.term_id()), equality_(equality) {
#ifdef TRANSFORMATIONAL
    reason_ = &reason;
    Reason::register_use(reason_);
#endif
  }


  /* Constructs a variable binding. */
  Binding(const Variable& var, size_t var_id, const Term& term, size_t term_id,
	  bool equality, const Reason& reason)
    : var_(&var), var_id_(var_id), term_(&term), term_id_(term_id),
      equality_(equality) {
#ifdef TRANSFORMATIONAL
    reason_ = &reason;
    Reason::register_use(reason_);
#endif
  }

  /* Constructs a variable binding. */
  Binding(const Binding& b)
    : var_(b.var_), var_id_(b.var_id_), term_(b.term_), term_id_(b.term_id_),
      equality_(b.equality_) {
#ifdef TRANSFORMATIONAL
    reason_ = b.reason_;
    Reason::register_use(reason_);
#endif
  }

  /* Deletes this variable binding. */
  ~Binding() {
#ifdef TRANSFORMATIONAL
    Reason::unregister_use(reason_);
#endif
  }

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

  friend std::ostream& operator<<(std::ostream& os, const Binding& b);
};

/* Output operator for variable bindings. */
std::ostream& operator<<(std::ostream& os, const Binding& b);


/* ====================================================================== */
/* BindingList */

/*
 * List of bindings.
 */
struct BindingList : std::vector<Binding> {
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
struct NameSet : public std::set<const Name*> {
  /* Register use of the given object. */
  static void register_use(const NameSet* s) {
    if (s != NULL) {
      s->ref_count_++;
    }
  }

  /* Unregister use of the given object. */
  static void unregister_use(const NameSet* s) {
    if (s != NULL) {
      s->ref_count_--;
      if (s->ref_count_ == 0) {
	delete s;
      }
    }
  }

  /* Constructs a name set. */
  NameSet();

  /* Deletes this name set. */
  ~NameSet();

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};

/* Iterator for name sets. */
typedef NameSet::const_iterator NameSetIter;


/* ====================================================================== */
/* ActionDomain */

/*
 * Domain for action parameters.
 */
struct ActionDomain {
  /* Register use of this object. */
  static void register_use(const ActionDomain* a) {
    if (a != NULL) {
      a->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const ActionDomain* a) {
    if (a != NULL) {
      a->ref_count_--;
      if (a->ref_count_ == 0) {
	delete a;
      }
    }
  } 

 /* Constructs an action domain with a single tuple. */
  ActionDomain(const NameList& tuple);

  /* Deletes this action domain. */
  ~ActionDomain();

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
  struct TupleList : public std::vector<const NameList*> {
  };

  /* A tuple list iterator. */
  typedef TupleList::const_iterator TupleListIter;

  /* A projection map. */
  struct ProjectionMap : public hashing::hash_map<size_t, const NameSet*> {
  };

  /* A projection map iterator. */
  typedef ProjectionMap::const_iterator ProjectionMapIter;

  /* Possible parameter tuples. */
  TupleList tuples_;
  /* Projections. */
  mutable ProjectionMap projections_;
  /* Reference counter. */
  mutable size_t ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const ActionDomain& ad);
};

/* Output operator for action domains. */
std::ostream& operator<<(std::ostream& os, const ActionDomain& ad);


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
struct Bindings {
  /* Register use of this object. */
  static void register_use(const Bindings* b) {
    if (b != NULL) {
      b->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const Bindings* b) {
    if (b != NULL) {
      b->ref_count_--;
      if (b->ref_count_ == 0) {
	delete b;
      }
    }
  }

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
  ~Bindings();

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
  /* Reference counter. */
  mutable size_t ref_count_;

  /* Constructs a binding collection. */
  Bindings(const VarsetChain* varsets, size_t high_step,
	   const StepDomainChain* step_domains,
	   const BindingChain* equalities, const BindingChain* inequalities);

  friend std::ostream& operator<<(std::ostream& os, const Bindings& b);
};

/* Output operator for bindings. */
std::ostream& operator<<(std::ostream& os, const Bindings& b);


#endif /* BINDINGS_H */
