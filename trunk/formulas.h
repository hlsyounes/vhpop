/* -*-C++-*- */
/*
 * Formulas.
 *
 * $Id: formulas.h,v 1.7 2001-07-29 17:57:58 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <string>
#include <hash_map>
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

  friend ostream& operator<<(ostream& os, const Term& t);
  friend bool operator==(const Term& t1, const Term& t2);
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
 * Name.
 */
struct Name : public Term {
  /* Constructs a name. */
  Name(const string& name, const Type& type = SimpleType::OBJECT_TYPE)
    : Term(name, type) {
  }

  /* Returns an instantiation of this term. */
  virtual const Name& instantiation(size_t id) const {
    return *this;
  }

  /* Returns this term subject to the given substitutions. */
  virtual const Name& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Checks if this term is equivalent to the given term; two terms
     are equivalent if they both are the same name, or if they are
     variables (variable names only matter for equality and not for
     equivalence). */
  virtual bool equivalent(const Term& t) const {
    return *this == t;
  }

protected:
  /* Checks if this term equals the given term. */
  virtual bool equals(const Term& t) const {
    const Name* nt = dynamic_cast<const Name*>(&t);
    return nt != NULL && name == nt->name;
  }
};


/*
 * Variable.
 */
struct Variable : public Term {
  /* Constructs a variable with the given name. */
  Variable(const string& name, const Type& type = SimpleType::OBJECT_TYPE)
    : Term(name, type) {
  }

  /* Returns an instantiation of this variable. */
  virtual const Variable& instantiation(size_t id) const;

  /* Returns this variable subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const {
    for (SubstitutionList::const_iterator i = subst.begin();
	 i != subst.end(); i++) {
      const Substitution& s = **i;
      if (*this == s.var) {
	return s.term;
      }
    }
    return *this;
  }

  /* Checks if this variable is equivalent to the given term. */
  virtual bool equivalent(const Term& t) const {
    const Variable* v = dynamic_cast<const Variable*>(&t);
    return v != NULL;
  }

protected:
  /* Checks if this variable equals the given term. */
  virtual bool equals(const Term& t) const {
    const Variable* vt = dynamic_cast<const Variable*>(&t);
    return vt != NULL && name == vt->name;
  }
};


/*
 * Instantiated variable.
 */
struct StepVar : public Variable {
  /* Construct an instantiated variable. */
  StepVar(const Variable& var, size_t id)
    : Variable(var), id_(id) {
  }

protected:
  /* Prints this instantiated variable on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this instantiated variable equals the given term. */
  virtual bool equals(const Term& t) const {
    const StepVar* vt = dynamic_cast<const StepVar*>(&t);
    return vt != NULL && name == vt->name && id_ == vt->id_;
  }

private:
  /* The id of the step that this variable belongs to. */
  size_t id_;
};


/* Returns an instantiation of this variable. */
inline const Variable& Variable::instantiation(size_t id) const {
  return *(new StepVar(*this, id));
}


/*
 * List of terms.
 */
struct TermList : public gc, vector<const Term*, container_alloc> {
  /* Constructs an empty term list. */
  TermList() {
  }

  /* Returns an instantiation of this term list. */
  const TermList& instantiation(size_t id) const {
    TermList& terms = *(new TermList());
    for (const_iterator i = begin(); i != end(); i++) {
      terms.push_back(&(*i)->instantiation(id));
    }
    return terms;
  }

  /* Returns this term list subject to the given substitutions. */
  const TermList& substitution(const SubstitutionList& subst) const {
    TermList& terms = *(new TermList());
    for (const_iterator i = begin(); i != end(); i++) {
      terms.push_back(&(*i)->substitution(subst));
    }
    return terms;
  }

  /* Checks if this term list is equivalent to the given term list. */
  bool equivalent(const TermList& terms) const {
    if (size() != terms.size()) {
      return false;
    } else {
      for (const_iterator i = begin(), j = terms.begin();
	   i != end(); i++, j++) {
	if (!(*i)->equivalent(**j)) {
	  return false;
	}
      }
      return true;
    }
  }

  /* Equality operator for term lists. */
  bool operator==(const TermList& terms) const {
    if (size() != terms.size()) {
      return false;
    } else {
      for (const_iterator i = begin(), j = terms.begin();
	   i != end(); i++, j++) {
	if (**i != **j) {
	  return false;
	}
      }
      return true;
    }
  }

  /* Inequality operator for term lists. */
  bool operator!=(const TermList& terms) const {
    return !(*this == terms);
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
  /* Constructs an empty variable list. */
  VariableList() {
  }

  /* Checks if this variable list contains the given variable. */
  bool contains(const Variable& v) const {
    for (const_iterator i = begin(); i != end(); i++) {
      if (**i == v) {
	return true;
      }
    }
    return false;
  }

  /* Returns an instantiation of this variable list. */
  const VariableList& instantiation(size_t id) const {
    VariableList& variables = *(new VariableList());
    for (const_iterator i = begin(); i != end(); i++) {
      variables.push_back(&(*i)->instantiation(id));
    }
    return variables;
  }

  /* Equality operator for variable lists. */
  bool operator==(const VariableList& variables) const {
    if (size() != variables.size()) {
      return false;
    } else {
      for (const_iterator i = begin(), j = variables.begin();
	   i != end(); i++, j++) {
	if (**i != **j) {
	  return false;
	}
      }
      return true;
    }
  }

  /* Inequality operator for variable lists. */
  bool operator!=(const VariableList& variables) const {
    return !(*this == variables);
  }
};


struct Problem;

/*
 * Abstract formula.
 */
struct Formula : public gc {
  /* Deletes this formula. */
  virtual ~Formula() {
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const = 0;

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const = 0;

  /* Checks if this formula matches the given formula; two formulas
     matches if they are both atomic formulas, or negations of atomic
     formulas, and the predicates of the atomic formulas are the
     same. */
  virtual bool matches(const Formula& f) const {
    return false;
  }

  /* Checks if this formula is equivalent to the given formula; two
     formulas are equivalent if they match (as defined above), and the
     term lists of the atomic formulas are equivalent. */
  virtual bool equivalent(const Formula& f) const {
    return false;
  }

  /* Checks if this formula negates the given formula. */
  virtual bool negates(const Formula& f) const {
    return false;
  }

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const = 0;

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const = 0;

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const = 0;

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const = 0;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const = 0;

  friend ostream& operator<<(ostream& os, const Formula& f);
  friend bool operator==(const Formula& f1, const Formula& f2);
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
  const FormulaList& instantiation(size_t id) const {
    FormulaList& formulas = *(new FormulaList());
    for (const_iterator i = begin(); i != end(); i++) {
      formulas.push_back(&(*i)->instantiation(id));
    }
    return formulas;
  }

  /* Returns this formula list subject to the given substitutions. */
  const FormulaList& substitution(const SubstitutionList& subst) const {
    FormulaList& formulas = *(new FormulaList());
    for (const_iterator i = begin(); i != end(); i++) {
      formulas.push_back(&(*i)->substitution(subst));
    }
    return formulas;
  }

  /* Returns a negation of this formula list. */
  const FormulaList& negation() const {
    FormulaList& formulas = *(new FormulaList());
    for (const_iterator i = begin(); i != end(); i++) {
      formulas.push_back(&(*i)->negation());
    }
    return formulas;
  }

  /* Checks if any of the formulas in this list matches the given formula. */
  bool matches(const Formula& f) const {
    for (const_iterator i = begin(); i != end(); i++) {
      if ((*i)->matches(f)) {
	return true;
      }
    }
    return false;
  }

  /* Checks if any of the formulas in this list involves the given
   * predicate. */
  bool involves(const string& predicate) const {
    for (const_iterator i = begin(); i != end(); i++) {
      if ((*i)->involves(predicate)) {
	return true;
      }
    }
    return false;
  }

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
  bool operator==(const FormulaList& formulas) const {
    if (size() != formulas.size()) {
      return false;
    } else {
      for (const_iterator i = begin(), j = formulas.begin();
	   i != end(); i++, j++) {
	if (**i != **j) {
	  return false;
	}
      }
      return true;
    }
  }

  /* Inequality operator for formula lists. */
  bool operator!=(const FormulaList& formulas) const {
    return !(*this == formulas);
  }
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

  /* Returns an instantiation of this atomic formula. */
  virtual const AtomicFormula& instantiation(size_t id) const {
    return *(new AtomicFormula(predicate, terms.instantiation(id)));
  }

  /* Returns this atomic formula subject to the given substitutions. */
  virtual const AtomicFormula&
  substitution(const SubstitutionList& subst) const {
    return *(new AtomicFormula(predicate, terms.substitution(subst)));
  }

  /* Returns a negation of this atomic formula. */
  virtual const Formula& negation() const;

  /* Checks if this atomic formula matches the given formula. */
  virtual bool matches(const Formula& f) const {
    const AtomicFormula* atom = dynamic_cast<const AtomicFormula*>(&f);
    return atom != NULL && predicate == atom->predicate;
  }

  /* Checks if this atomic formula is equivalent to the given formula. */
  virtual bool equivalent(const Formula& f) const {
    const AtomicFormula* atom = dynamic_cast<const AtomicFormula*>(&f);
    return (atom != NULL && predicate == atom->predicate
	    && terms.equivalent(atom->terms));
  }

  /* Checks if this atomic formula negates the given formula. */
  virtual bool negates(const Formula& f) const;

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return this->predicate == predicate;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const;

protected:
  /* Prints this atomic formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this atomic formula equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const AtomicFormula* atom = dynamic_cast<const AtomicFormula*>(&f);
    return (atom != NULL &&
	    predicate == atom->predicate && terms == atom->terms);
  }
};


/*
 * Negated atomic formula.
 */
struct Negation : public Formula {
  /* The negated atomic formula. */
  const AtomicFormula& atom;

  /* Constructs a negated atomic formula. */
  Negation(const AtomicFormula& atom)
    : atom(atom) {
  }

  /* Returns an instantiation of this negation. */
  virtual const Negation& instantiation(size_t id) const {
    return *(new Negation(atom.instantiation(id)));
  }

  /* Returns this negation subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionList& subst) const {
    return *(new Negation(atom.substitution(subst)));
  }

  /* Returns an atomic formula that this is a negation of. */
  virtual const AtomicFormula& negation() const {
    return atom;
  }

  /* Checks if this negation matches the given formula. */
  virtual bool matches(const Formula& f) const {
    const Negation* negation = dynamic_cast<const Negation*>(&f);
    return negation != NULL && atom.matches(negation->atom);
  }

  /* Checks if this negation is equivalent to the given formula. */
  virtual bool equivalent(const Formula& f) const {
    const Negation* negation = dynamic_cast<const Negation*>(&f);
    return negation != NULL && atom.equivalent(negation->atom);
  }

  /* Checks if this is a negation of the given formula. */
  virtual bool negates(const Formula& f) const {
    return atom == f;
  }

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return atom.involves(predicate);
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    return !atom.consistent(problem);
  }

protected:
  /* Prints this negation on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this negation equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const Negation* negation = dynamic_cast<const Negation*>(&f);
    return negation != NULL && atom == negation->atom;
  }
};


/* Returns a negation of this atomic formula. */
inline const Formula& AtomicFormula::negation() const {
  return *(new Negation(*this));
}

/* Checks if this atomic formula negates the given formula. */
inline bool AtomicFormula::negates(const Formula& f) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  return negation != NULL && negation->negates(*this);
}


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

  /* Returns the instantiation of this equality. */
  virtual const Equality& instantiation(size_t id) const {
    return *(new Equality(term1.instantiation(id), term2.instantiation(id)));
  }

  /* Returns this equality subject to the given substitutions. */
  virtual const Equality& substitution(const SubstitutionList& subst) const {
    return *(new Equality(term1.substitution(subst),
			  term2.substitution(subst)));
  }

  /* Returns a negation of this equality. */
  virtual const Formula& negation() const;

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return "=" == predicate;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    return term1 == term2;
  }

protected:
  /* Prints this equality on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this equality equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const Equality* eq = dynamic_cast<const Equality*>(&f);
    return (eq != NULL &&
	    ((term1 == eq->term1 && term2 == eq->term2) ||
	     (term1 == eq->term2 && term2 == eq->term1)));
  }
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

  /* Returns an instantiation of this inequality. */
  virtual const Inequality& instantiation(size_t id) const {
    return *(new Inequality(term1.instantiation(id), term2.instantiation(id)));
  }

  /* Returns this inequality subject to the given substitutions. */
  virtual const Inequality& substitution(const SubstitutionList& subst) const {
    return *(new Inequality(term1.substitution(subst),
			    term2.substitution(subst)));
  }

  /* Returns a negation of this inequality formula. */
  virtual const Formula& negation() const {
    return *(new Equality(term1, term2));
  }

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return "=" == predicate;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    return term1 != term2;
  }

protected:
  /* Prints this inequality on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this inequality equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const Inequality* neq = dynamic_cast<const Inequality*>(&f);
    return (neq != NULL &&
	    ((term1 == neq->term1 && term2 == neq->term2) ||
	     (term1 == neq->term2 && term2 == neq->term1)));
  }
};


/* Returns a negation of this equality. */
inline const Formula& Equality::negation() const {
  return *(new Inequality(term1, term2));
}


/*
 * Conjunction.
 */
struct Conjunction : public Formula {
  /* The conjuncts. */
  const FormulaList& conjuncts;

  /* Constructs a conjunction. */
  Conjunction(const FormulaList& conjuncts)
    : conjuncts(conjuncts) {
  }

  /* Returns an instantiation of this conjunction. */
  virtual const Conjunction& instantiation(size_t id) const {
    return *(new Conjunction(conjuncts.instantiation(id)));
  }

  /* Returns this conjunction subject to the given substitutions. */
  virtual const Conjunction&
  substitution(const SubstitutionList& subst) const {
    return *(new Conjunction(conjuncts.substitution(subst)));
  }

  /* Returns the negation of this conjunction. */
  virtual const Formula& negation() const;

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return conjuncts.involves(predicate);
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return conjuncts.cost();
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    for (FormulaList::const_iterator c = conjuncts.begin();
	 c != conjuncts.end(); c++) {
      if (!(*c)->consistent(problem)) {
	return false;
      }
    }
    return true;
  }

protected:
  /* Prints this conjunction on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this conjunction equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const Conjunction* conjunction = dynamic_cast<const Conjunction*>(&f);
    return conjunction != NULL && conjuncts == conjunction->conjuncts;
  }
};


/*
 * Disjunction.
 */
struct Disjunction : public Formula {
  /* The disjuncts. */
  const FormulaList& disjuncts;

  /* Constructs a disjunction. */
  Disjunction(const FormulaList& disjuncts)
    : disjuncts(disjuncts) {
  }

  /* Returns an instantiation of this disjunction. */
  virtual const Disjunction& instantiation(size_t id) const {
    return *(new Disjunction(disjuncts.instantiation(id)));
  }

  /* Returns this disjunction subject to the given substitution. */
  virtual const Disjunction&
  substitution(const SubstitutionList& subst) const {
    return *(new Disjunction(disjuncts.substitution(subst)));
  }

  /* Returns a negation of this disjunction. */
  virtual const Formula& negation() const {
    return *(new Conjunction(disjuncts.negation()));
  }

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return disjuncts.involves(predicate);
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    for (FormulaList::const_iterator c = disjuncts.begin();
	 c != disjuncts.end(); c++) {
      if ((*c)->consistent(problem)) {
	return true;
      }
    }
    return false;
  }

protected:
  /* Prints this disjunction on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this disjunction equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const Disjunction* disjunction = dynamic_cast<const Disjunction*>(&f);
    return disjunction != NULL && disjuncts == disjunction->disjuncts;
  }
};


/* Returns the negation of this conjunction. */
inline const Formula& Conjunction::negation() const {
  return *(new Disjunction(conjuncts.negation()));
}


/*
 * Existentially quantified formula.
 */
struct ExistsFormula : public Formula {
  /* Existentially quanitfied variables. */
  const VariableList& parameters;
  /* The quantified formula. */
  const Formula& body;

  /* Constructs an existentially quantified formula. */
  ExistsFormula(const VariableList& parameters, const Formula& body)
    : parameters(parameters), body(body) {
  }

  /* Returns an instantiation of this formula. */
  virtual const ExistsFormula& instantiation(size_t id) const {
    return *(new ExistsFormula(parameters.instantiation(id),
			       body.instantiation(id)));
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const ExistsFormula&
  substitution(const SubstitutionList& subst) const {
    return *(new ExistsFormula(parameters, body.substitution(subst)));
  }

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const;

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return body.involves(predicate);
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    return body.consistent(problem);
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const ExistsFormula* exists = dynamic_cast<const ExistsFormula*>(&f);
    return (exists != NULL &&
	    parameters == exists->parameters && body == exists->body);
  }
};


/*
 * Universally quantified formula.
 */
struct ForallFormula : public Formula {
  /* Existentially quantified variables. */
  const VariableList& parameters;
  /* The quantified formula. */
  const Formula& body;

  /* Constructs a universally quantified formula. */
  ForallFormula(const VariableList& parameters, const Formula& body)
    : parameters(parameters), body(body) {
  }

  /* Returns an instantiation of this formula. */
  virtual const ForallFormula& instantiation(size_t id) const {
    return *(new ForallFormula(parameters.instantiation(id),
			       body.instantiation(id)));
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const ForallFormula&
  substitution(const SubstitutionList& subst) const {
    return *(new ForallFormula(parameters, body.substitution(subst)));
  }

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const {
    return *(new ExistsFormula(parameters, body.negation()));
  }

  /* Checks if this formula involves the given predicate. */
  virtual bool involves(const string& predicate) const {
    return body.involves(predicate);
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 1;
  }

  /* Checks if this formula is consistent. */
  virtual bool consistent(const Problem& problem) const {
    return body.consistent(problem);
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const {
    const ForallFormula* forall = dynamic_cast<const ForallFormula*>(&f);
    return (forall != NULL &&
	    parameters == forall->parameters && body == forall->body);
  }
};


/* Returns a negation of this formula. */
inline const Formula& ExistsFormula::negation() const {
  return *(new ForallFormula(parameters, body.negation()));
}

#endif /* FORMULAS_H */
