/* -*-C++-*- */
/*
 * Binding constraints.
 *
 * $Id: bindings.h,v 1.8 2001-10-30 18:41:13 lorens Exp $
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
struct Formula;
struct Equality;
struct Inequality;
struct Reason;
struct Step;
struct PlanningGraph;

/*
 * Abstract variable binding.
 */
struct Binding : public Printable, public gc {
  /* A variable. */
  const Variable& variable;
  /* A term either bound to, or separated from the variable. */
  const Term& term;
  /* Reason for binding. */
  const Reason& reason;

protected:
  /* Constructs an abstract variable binding. */
  Binding(const Variable& variable, const Term& term, const Reason& reason)
    : variable(variable), term(term), reason(reason) {
  }
};


/*
 * Equality binding.
 */
struct EqualityBinding : public Binding {
  /* Constructs an equality binding from the given substitution. */
  EqualityBinding(const Substitution& s, const Reason& reason);

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
  InequalityBinding(const Substitution& s, const Reason& reason);

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
struct BindingList : Vector<const Binding*> {
};


/*
 * Chain of bindings.
 */
typedef Chain<const Binding*> BindingChain;


/*
 * A set of names.
 */
struct NameSet : public Set<const Name*, less<const LessThanComparable*> > {
};


/*
 * A set of variables.
 */
struct VariableSet
  : public Set<const Variable*, less<const LessThanComparable*> > {
};


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


/*
 * A collection of variable bindings.
 */
struct Bindings : public Printable, public gc {
  /* Equality bindings. */
  const BindingChain* const equalities;
  /* Inequality bindings. */
  const BindingChain* const inequalities;

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
  static bool unifiable(const Formula& f1, const Formula& f2);

  /* Returns the binding for the given term, or the term itself if it
     is not bound to a single name. */
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

  /* Returns the binding collection obtained by adding the constraints
     associated with the given step to this binding collection, or
     NULL if the new binding collection would be inconsistent. */
  const Bindings* add(const Step& step, const PlanningGraph& pg) const;

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

  /* Constructs a binding collection. */
  Bindings(const BindingChain* equalities, const BindingChain* inequalities,
	   const VarsetChain* varsets, size_t high_step,
	   const StepDomainChain* step_domains)
    : equalities(equalities), inequalities(inequalities),
      varsets_(varsets), high_step_(high_step), step_domains_(step_domains) {
  }
};

#endif /* BINDINGS_H */
