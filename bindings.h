/* -*-C++-*- */
/*
 * Binding constraints.
 *
 * $Id: bindings.h,v 1.2 2001-07-29 17:38:25 lorens Exp $
 */
#ifndef BINDINGS_H
#define BINDINGS_H

#include <iostream>
#include "support.h"
#include "formulas.h"


struct Reason;


/*
 * Abstract variable binding.
 */
struct Binding : public gc {
  /* A variable. */
  const Variable& variable;
  /* A term either bound to, or separated from the variable. */
  const Term& term;
  /* Reason for binding. */
  const Reason& reason;

  /* Deletes a binding. */
  virtual ~Binding() {
  }

protected:
  /* Constructs an abstract variable binding. */
  Binding(const Variable& variable, const Term& term, const Reason& reason)
    : variable(variable), term(term), reason(reason) {
  }

  /* Prints this binding on the given stream. */
  virtual void print(ostream& os) const = 0;

  friend ostream& operator<<(ostream& os, const Binding& b);
};

/* Output operator for bindings. */
inline ostream& operator<<(ostream& os, const Binding& b) {
  b.print(os);
  return os;
}


/*
 * Equality binding.
 */
struct EqualityBinding : public Binding {
  /* Constructs an equality binding from the given substitution. */
  EqualityBinding(const Substitution& s, const Reason& reason)
    : Binding(s.var, s.term, reason) {
  }

  /* Construct an equality binding, binding the given variable to the
     given term. */
  EqualityBinding(const Variable& variable, const Term& term,
		  const Reason& reason)
    : Binding(variable, term, reason) {
  }

protected:
  /* Prints this equality binding on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Inequality binding.
 */
struct InequalityBinding : public Binding {
  /* Constructs an inequality binding from the given substitution. */
  InequalityBinding(const Substitution& s, const Reason& reason)
    : Binding(s.var, s.term, reason) {
  }

  /* Constructs an inequality binding, separating the given variable
     from the given term. */
  InequalityBinding(const Variable& variable, const Term& term,
		    const Reason& reason)
    : Binding(variable, term, reason) {
  }

protected:
  /* Prints this inequality binding on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * List of bindings.
 */
typedef vector<const Binding*, container_alloc> BindingList;


/*
 * Chain of bindings.
 */
typedef Chain<const Binding*> BindingChain;


struct Varset;

/*
 * Chain of varsets.
 */
typedef Chain<const Varset*> VarsetChain;


/*
 * A collection of variable bindings.
 */
struct Bindings : public gc {
  /* Equality bindings. */
  const BindingChain* const equalities;
  /* Inequality bindings. */
  const BindingChain* const inequalities;

  /* Creates a collection of variable bindings with the given equality
     and inequality bindings. */
  static const Bindings* make_bindings(const BindingChain* equalities,
				       const BindingChain* inequalities);

  /* Constructs an empty binding collection. */
  Bindings()
    : equalities(NULL), inequalities(NULL), varsets_(NULL) {
  }

  /* Returns an instantiation of the given formula, where bound
     variables have been substituted for the value they are bound
     to. */
  const Formula& instantiation(const Formula& f) const;

  /* Returns the binding for the given term, or the term itself if it
     is unbound. */
  const Term& binding(const Term& t) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified. */
  bool affects(const Formula& f1, const Formula& f2) const;

  /* Checks if one of the given formulas is the negation of the other,
     and the atomic formulas can be unified; the most general unifier
     is added to the provided substitution list. */
  bool affects(SubstitutionList& mgu,
	       const Formula& f1, const Formula& f2) const;

  /* Checks if the given formulas can be unified. */
  bool unify(const Formula& f1, const Formula& f2) const;

  /* Checks if the given formulas can be unified; the most general
     unifier is added to the provided substitution list. */
  bool unify(SubstitutionList& mgu,
	     const Formula& f1, const Formula& f2) const;

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

private:
  /* Varsets representing the transitive closure of the bindings. */
  const VarsetChain* const varsets_;

  /* Constructs a binding collection. */
  Bindings(const BindingChain* equalities, const BindingChain* inequalities,
	   const VarsetChain* varsets)
    : equalities(equalities), inequalities(inequalities), varsets_(varsets) {
  }

  /* Prints this binding collection on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Bindings& b);
};

/* Output operator for binding collections. */
inline ostream& operator<<(ostream& os, const Bindings& b) {
  b.print(os);
  return os;
}

#endif /* BINDINGS_H */
