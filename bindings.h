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
// Binding constraints.

#ifndef BINDINGS_H
#define BINDINGS_H

#include <set>

#include "chain.h"
#include "terms.h"

struct Literal;
struct Equality;
struct Inequality;
struct Action;
struct Problem;
struct Step;
struct PlanningGraph;


/* ====================================================================== */
/* Binding */

/*
 * Variable binding.
 */
struct Binding {
  /* Constructs a variable binding. */
  Binding(const Variable& var, size_t var_id, const Term& term, size_t term_id,
	  bool equality)
    : var_(var), var_id_(var_id), term_(term), term_id_(term_id),
      equality_(equality) {}

  /* Constructs a variable binding. */
  Binding(const Binding& b)
    : var_(b.var_), var_id_(b.var_id_), term_(b.term_), term_id_(b.term_id_),
      equality_(b.equality_) {}

  /* Returns the variable of this binding. */
  const Variable& var() const { return var_; }

  /* Returns the step id for the variable. */
  size_t var_id() const { return var_id_; }

  /* Returns the term of this binding. */
  const Term& term() const { return term_; }

  /* Return the step id for the term. */
  size_t term_id() const { return term_id_; }

  /* Checks if this is an equality binding. */
  bool equality() const { return equality_; }

private:
  /* A variable. */
  Variable var_;
  /* Step id for the variable. */
  size_t var_id_;
  /* A term either bound to, or separated from the variable. */
  Term term_;
  /* Step id for the term. */
  size_t term_id_;
  /* Whether or not this is an equality binding. */
  bool equality_;
};


/* ====================================================================== */
/* BindingList */

/*
 * List of bindings.
 */
struct BindingList : std::vector<Binding> {
};


/* ====================================================================== */
/* NameSet */

/*
 * A set of names.
 */
struct NameSet : public std::set<Object> {
};


/* ====================================================================== */
/* TupleList */

/*
 * A list of parameter tuples.
 */
struct TupleList : public std::vector<const std::vector<Object>*> {};

/* ====================================================================== */
/* ActionDomain */

/*
 * Domain for action parameters.
 */
struct ActionDomain {
  /* Register use of this object. */
  static void register_use(const ActionDomain* a) {
    if (a != 0) {
      a->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const ActionDomain* a) {
    if (a != 0) {
      a->ref_count_--;
      if (a->ref_count_ == 0) {
	delete a;
      }
    }
  } 

 /* Constructs an action domain with a single tuple. */
  ActionDomain(const std::vector<Object>& tuple);

  /* Deletes this action domain. */
  ~ActionDomain();

  /* Number of tuples. */
  size_t size() const { return tuples().size(); }

  /* Returns the tuples of this action domain. */
  const TupleList& tuples() const { return tuples_; }

  /* Adds a tuple to this domain. */
  void add(const std::vector<Object>& tuple);

  /* Returns the set of names from the given column. */
  const NameSet& projection(size_t column) const;

  /* Returns the size of the projection of the given column. */
  const size_t projection_size(size_t column) const;

  /* Returns a domain where the given column has been restricted to
     the given object, or 0 if this would leave an empty domain. */
  const ActionDomain* restrict(const Object& obj, size_t column) const;

  /* Returns a domain where the given column has been restricted to
     the given set of names, or 0 if this would leave an empty
     domain. */
  const ActionDomain* restrict(const NameSet& names, size_t column) const;

  /* Returns a domain where the given column exclues the given object,
     or 0 if this would leave an empty domain. */
  const ActionDomain* exclude(const Object& obj, size_t column) const;

  /* Prints this object on the given stream. */
  void print(std::ostream& os) const;

private:
  /* A projection map. */
  struct ProjectionMap : public std::map<size_t, const NameSet*> {
  };

  /* Possible parameter tuples. */
  TupleList tuples_;
  /* Projections. */
  mutable ProjectionMap projections_;
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Bindings */

struct Varset;
struct StepDomain;

/*
 * A collection of variable bindings.
 */
struct Bindings {
  /* Empty bindings. */
  static const Bindings EMPTY;

  /* Register use of this object. */
  static void register_use(const Bindings* b) {
    if (b != 0) {
      b->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const Bindings* b) {
    if (b != 0) {
      b->ref_count_--;
      if (b->ref_count_ == 0) {
	delete b;
      }
    }
  }

  /* Checks if the given formulas can be unified. */
  static bool unifiable(const Literal& l1, size_t id1,
			const Literal& l2, size_t id2);

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  static bool unifiable(BindingList& mgu,
			const Literal& l1, size_t id1,
			const Literal& l2, size_t id2);

  /* Deletes this binding collection. */
  ~Bindings();

  /* Returns the binding for the given term, or the term itself if it
     is not bound to a single object. */
  Term binding(const Term& term, size_t step_id) const;

  /* Returns the domain for the given step variable. */
  const NameSet& domain(const Variable& var, size_t step_id,
			const Problem& problem) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified. */
  bool affects(const Literal& l1, size_t id1,
	       const Literal& l2, size_t id2) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified; the most general unifier
     is added to the provided substitution list. */
  bool affects(BindingList& mgu, const Literal& l1, size_t id1,
	       const Literal& l2, size_t id2) const;

  /* Checks if the given formulas can be unified. */
  bool unify(const Literal& l1, size_t id1,
	     const Literal& l2, size_t id2) const;

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  bool unify(BindingList& mgu, const Literal& l1, size_t id1,
	     const Literal& l2, size_t id2) const;

  /* Checks if the given equality is consistent with the current
     bindings. */
  bool consistent_with(const Equality& eq, size_t step_id) const;

  /* Checks if the given inequality is consistent with the current
     bindings. */
  bool consistent_with(const Inequality& neq, size_t step_id) const;

  /* Returns the binding collection obtained by adding the given
     bindings to this binding collection, or 0 if the new bindings
     are inconsistent with the current. */
  const Bindings* add(const BindingList& new_bindings,
		      bool test_only = false) const;

  /* Returns the binding collection obtained by adding the constraints
     associated with the given step to this binding collection, or
     0 if the new binding collection would be inconsistent. */
  const Bindings* add(size_t step_id, const Action& step_action,
		      const PlanningGraph& pg, bool test_only = false) const;

  /* Prints this object on the given stream. */
  void print(std::ostream& os) const;

  /* Prints the given term on the given stream. */
  void print_term(std::ostream& os, const Term& term, size_t step_id) const;

private:
  /* Varsets representing the transitive closure of the bindings. */
  const Chain<Varset>* varsets_;
  /* Highest step id of variable in varsets. */
  size_t high_step_;
  /* Step domains. */
  const Chain<StepDomain>* step_domains_;
  /* Reference counter. */
  mutable size_t ref_count_;

  /* Constructs an empty binding collection. */
  Bindings();

  /* Constructs a binding collection. */
  Bindings(const Chain<Varset>* varsets, size_t high_step,
	   const Chain<StepDomain>* step_domains);
};


#endif /* BINDINGS_H */
