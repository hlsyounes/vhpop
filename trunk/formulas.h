/* -*-C++-*- */
/*
 * Formulas.
 *
 * $Id: formulas.h,v 1.11 2001-08-18 15:46:10 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <string>
#include <hash_map>
#include <hash_set>
#include <vector>
#include "support.h"
#include "types.h"


struct Term;
struct Variable;

/*
 * Variable substitution.
 */
struct Substitution : public gc {
  /* Variable to get substituted. */
  const Variable& var;
  /* Term to substitute with. */
  const Term& term;

  /* Constructs a substitution. */
  Substitution(const Variable& var, const Term& term)
    : var(var), term(term) {
  }

private:
  /* Prints this substitution on the given stream. */
  virtual void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Substitution& s);
};

/* Output operator for substitutions. */
inline ostream& operator<<(ostream& os, const Substitution& s) {
  s.print(os);
  return os;
}


/*
 * List of substitutions.
 */
struct SubstitutionList : public gc,
			  vector<const Substitution*, container_alloc> {
};


/*
 * Abstract term.
 */
struct Term : public gc {
  /* Name of term. */
  const string name;
  /* Type of term. */
  const Type& type;

  /* Constructs an abstract term with the given name. */
  Term(const string& name, const Type& type)
    : name(name), type(type) {
  }

  /* Deletes this term. */
  virtual ~Term() {
  }

  /* Returns an instantiation of this term. */
  virtual const Term& instantiation(size_t id) const = 0;

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const = 0;

  /* Checks if this term is equivalent to the given term; two terms
     are equivalent if they both are the same name, or if they are
     variables (variable names only matter for equality and not for
     equivalence). */
  virtual bool equivalent(const Term& t) const = 0;

protected:
  /* Prints this term on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this term equals the given term. */
  virtual bool equals(const Term& t) const = 0;

  /* Returns the hash value of this term. */
  virtual size_t hash_value() const;

  friend ostream& operator<<(ostream& os, const Term& t);
  friend bool operator==(const Term& t1, const Term& t2);
  friend struct hash<Term>;
};

/* Output operator for terms. */
inline ostream& operator<<(ostream& os, const Term& t) {
  t.print(os);
  return os;
}

/* Equality operator for terms. */
inline bool operator==(const Term& t1, const Term& t2) {
  return t1.equals(t2);
}

/* Inequality operator for terms. */
inline bool operator!=(const Term& t1, const Term& t2) {
  return !(t1 == t2);
}

/*
 * Hash function object for terms.
 */
struct hash<Term> {
  size_t operator()(const Term& t) const {
    return t.hash_value();
  }
};


/*
 * Name.
 */
struct Name : public Term {
  /* Constructs a name. */
  Name(const string& name, const Type& type = SimpleType::OBJECT)
    : Term(name, type) {
  }

  /* Returns an instantiation of this term. */
  virtual const Name& instantiation(size_t id) const;

  /* Returns this term subject to the given substitutions. */
  virtual const Name& substitution(const SubstitutionList& subst) const;

  /* Checks if this term is equivalent to the given term; two terms
     are equivalent if they both are the same name, or if they are
     variables (variable names only matter for equality and not for
     equivalence). */
  virtual bool equivalent(const Term& t) const;

protected:
  /* Checks if this term equals the given term. */
  virtual bool equals(const Term& t) const;
};


/*
 * Variable.
 */
struct Variable : public Term {
  /* Constructs a variable with the given name. */
  Variable(const string& name, const Type& type = SimpleType::OBJECT)
    : Term(name, type) {
  }

  /* Returns an instantiation of this term. */
  virtual const Variable& instantiation(size_t id) const;

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const;

  /* Checks if this variable is equivalent to the given term. */
  virtual bool equivalent(const Term& t) const;

protected:
  /* Checks if this term equals the given term. */
  virtual bool equals(const Term& t) const;
};


/*
 * List of terms.
 */
struct TermList : public gc, vector<const Term*, container_alloc> {
  /* Constructs an empty term list. */
  TermList() {
  }

  /* Returns an instantiation of this term list. */
  const TermList& instantiation(size_t id) const;

  /* Returns this term list subject to the given substitutions. */
  const TermList& substitution(const SubstitutionList& subst) const;

  /* Checks if this term list is equivalent to the given term list. */
  bool equivalent(const TermList& terms) const;

  /* Equality operator for term lists. */
  bool operator==(const TermList& terms) const;

  /* Inequality operator for term lists. */
  bool operator!=(const TermList& terms) const;
};

/*
 * Hash function object for term lists.
 */
struct hash<TermList> {
  size_t operator()(const TermList& terms) const {
    hash<Term> h;
    size_t v = 0;
    for (TermList::const_iterator ti = terms.begin();
	 ti != terms.end(); ti++) {
      v += h(**ti);
    }
    return v;
  }
};


/*
 * List of names.
 */
struct NameList : public gc, vector<const Name*, container_alloc> {
};


/*
 * Table of names.
 */
struct NameMap : public gc,
		 hash_map<string, const Name*, hash<string>, equal_to<string>,
		 container_alloc> {
};


/*
 * List of variables.
 */
struct VariableList : public gc, vector<const Variable*, container_alloc> {
  /* An empty variable list. */
  static const VariableList& EMPTY;

  /* Constructs an empty variable list. */
  VariableList() {
  }

  /* Checks if this variable list contains the given variable. */
  bool contains(const Variable& v) const;

  /* Returns an instantiation of this variable list. */
  const VariableList& instantiation(size_t id) const;

  /* Equality operator for variable lists. */
  bool operator==(const VariableList& variables) const;

  /* Inequality operator for variable lists. */
  bool operator!=(const VariableList& variables) const;
};


struct Problem;
struct FormulaList;

/*
 * Abstract formula.
 */
struct Formula : public gc {
  /* The true formula. */
  static const Formula& TRUE;
  /* The false formula. */
  static const Formula& FALSE;

  /* Deletes this formula. */
  virtual ~Formula() {
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const = 0;

  /* Checks if this formula is equivalent to the given formula; two
     formulas are equivalent if they match (as defined above), and the
     term lists of the atomic formulas are equivalent. */
  virtual bool equivalent(const Formula& f) const {
    return false;
  }

  /* Checks if this formula negates the given formula.  N.B. Only
     works for atomic formulas and negated atomic formulas. */
  virtual bool negates(const Formula& f) const {
    return false;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const = 0;

  /* Fills the provided list with goals achievable by this formula. */
  virtual void achievable_goals(FormulaList& goals) const {
  }

  /* Fills the provided sets with predicates achievable by this
     formula.  N.B. Will only add predicates for atomic formulas that
     are not fully instantiated. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const {
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const = 0;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const = 0;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const = 0;

  /* Returns the hash value of this formula. */
  virtual size_t hash_value() const {
    return 0;
  }

  friend ostream& operator<<(ostream& os, const Formula& f);
  friend bool operator==(const Formula& f1, const Formula& f2);
  friend const Formula& operator!(const Formula& f);
  friend struct hash<Formula>;
};

/* Output operator for formulas. */
inline ostream& operator<<(ostream& os, const Formula& f) {
  f.print(os);
  return os;
}

/* Equality operator for formulas. */
inline bool operator==(const Formula& f1, const Formula& f2) {
  return f1.equals(f2);
}

/* Inequality operator for formulas. */
inline bool operator!=(const Formula& f1, const Formula& f2) {
  return !(f1 == f2);
}

/* Negation operator for formulas. */
inline const Formula& operator!(const Formula& f) {
  return f.negation();
}

/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2);

/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2);
    
/*
 * Hash function object for formulas.
 */
struct hash<Formula> {
  size_t operator()(const Formula& f) const {
    return f.hash_value();
  }
};

/*
 * Equality function object for formula pointers.
 */
struct equal_to<const Formula*> {
  bool operator()(const Formula* f1, const Formula* f2) const {
    return *f1 == *f2;
  }
};

/*
 * Hash function object for formula pointers.
 */
struct hash<const Formula*> {
  size_t operator()(const Formula* f) const {
    return hash<Formula>()(*f);
  }
};


/*
 * List of formulas.
 */
struct FormulaList : public gc, vector<const Formula*, container_alloc> {
  /* Constructs an empty formula list. */
  FormulaList() {
  }

  /* Constructs a formula list with a single formula. */
  FormulaList(const Formula* formula)
    : vector<const Formula*, container_alloc>(1, formula) {
  }

  /* Returns an instantiation of this formula list. */
  const FormulaList& instantiation(size_t id) const;

  /* Returns an instantiation of this formula list. */
  const FormulaList& instantiation(const SubstitutionList& subst,
				   const Problem& problem) const;

  /* Returns this formula list subject to the given substitutions. */
  const FormulaList& substitution(const SubstitutionList& subst) const;

  /* Fills the provided list with goals achievable by the formulas in
     this list. */
  void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by the
     formulas in this list. */
  void achievable_predicates(hash_set<string>& preds,
			     hash_set<string>& neg_preds) const;

  /* Roughly corresponds to the number of open conditions the formulas
     in this list will give rise to. */
  size_t cost() const {
    size_t n = 0;
    for (const_iterator i = begin(); i != end(); i++) {
      n += (*i)->cost();
    }
    return n;
  }

  /* Equality operator for formula lists. */
  bool operator==(const FormulaList& formulas) const;

  /* Inequality operator for formula lists. */
  bool operator!=(const FormulaList& formulas) const;

  /* Returns the negation of this formula list. */
  const FormulaList& operator!() const;
};


/*
 * Atomic formula.
 */
struct AtomicFormula : public Formula {
  /* Predicate of this atomic formula. */
  const string predicate;
  /* Terms of this atomic formula. */
  const TermList& terms;

  /* Constructs an atomic formula. */
  AtomicFormula(const string& predicate, const TermList& terms)
    : predicate(predicate), terms(terms) {
  }

  /* Returns an instantiation of this formula. */
  virtual const AtomicFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Checks if this atomic formula is equivalent to the given formula. */
  virtual bool equivalent(const Formula& f) const {
    const AtomicFormula* atom = dynamic_cast<const AtomicFormula*>(&f);
    return (atom != NULL && predicate == atom->predicate
	    && terms.equivalent(atom->terms));
  }

  /* Checks if this atomic formula negates the given formula. */
  virtual bool negates(const Formula& f) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Fills the provided list with goals achievable by this formula. */
  virtual void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by this
     formula. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const;

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

  /* Returns the hash value of this formula. */
  virtual size_t hash_value() const;
};


/*
 * Negated atomic formula.
 */
struct Negation : public Formula {
  /* The negated atomic formula. */
  const AtomicFormula& atom;

  /* Returns an instantiation of this formula. */
  virtual const Negation& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Checks if this negation is equivalent to the given formula. */
  virtual bool equivalent(const Formula& f) const {
    const Negation* negation = dynamic_cast<const Negation*>(&f);
    return negation != NULL && atom.equivalent(negation->atom);
  }

  /* Checks if this is a negation of the given formula. */
  virtual bool negates(const Formula& f) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Fills the provided list with goals achievable by this formula. */
  virtual void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by this
     formula. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const;

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

  /* Returns the hash value of this formula. */
  virtual size_t hash_value() const;

private:
  /* Constructs a negated atomic formula. */
  Negation(const AtomicFormula& atom)
    : atom(atom) {
  }

  friend const Formula& AtomicFormula::negation() const;
};


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
    : term1(term1), term2(term2) {
  }

  /* Returns the instantiation of this formula. */
  virtual const Equality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

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
    : term1(term1), term2(term2) {
  }

  /* Returns an instantiation of this formula. */
  virtual const Inequality& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

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
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return conjuncts.cost();
  }

  /* Fills the provided list with goals achievable by this formula. */
  virtual void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by this
     formula. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const;

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a conjunction. */
  Conjunction(const FormulaList& conjuncts)
    : conjuncts(conjuncts) {
  }

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
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Fills the provided list with goals achievable by this formula. */
  virtual void achievable_goals(FormulaList& goals) const;

  /* Fills the provided sets with predicates achievable by this
     formula. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const;

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constructs a disjunction. */
  Disjunction(const FormulaList& disjuncts)
    : disjuncts(disjuncts) {
  }

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

  /* Fills the provided sets with predicates achievable by this
     formula. */
  virtual void achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const;

protected:
  QuantifiedFormula(const VariableList& parameters, const Formula& body)
    : parameters(parameters), body(body) {
  }
};


/*
 * Existentially quantified formula.
 */
struct ExistsFormula : public QuantifiedFormula {
  /* Constructs an existentially quantified formula. */
  ExistsFormula(const VariableList& parameters, const Formula& body)
    : QuantifiedFormula(parameters, body) {
  }

  /* Returns an instantiation of this formula. */
  virtual const ExistsFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};


/*
 * Universally quantified formula.
 */
struct ForallFormula : public QuantifiedFormula {
  /* Constructs a universally quantified formula. */
  ForallFormula(const VariableList& parameters, const Formula& body)
    : QuantifiedFormula(parameters, body) {
  }

  /* Returns an instantiation of this formula. */
  virtual const ForallFormula& instantiation(size_t id) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;
};

#endif /* FORMULAS_H */
