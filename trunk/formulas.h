/* -*-C++-*- */
/*
 * Types, terms, and formulas.
 *
 * $Id: formulas.h,v 1.3 2001-05-04 03:46:28 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include <iostream>
#include <string>
#include <hash_map>
#include <vector>
#include "support.h"


/*
 * Abstract type.
 */
struct Type : public gc {
  /* Deletes this type. */
  virtual ~Type() {
  }

  /* Checks if this type is object type. */
  bool object() const;

  /* Checks if this type is compatible with the given type. */
  bool compatible(const Type& t) const {
    return subtype(t) || t.subtype(*this);
  }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const = 0;

protected:
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const = 0;

  friend ostream& operator<<(ostream& os, const Type& t);
};

/* Output operator for types. */
inline ostream& operator<<(ostream& os, const Type& t) {
  t.print(os);
  return os;
}


/*
 * List of types.
 */
typedef vector<const Type*, container_alloc> TypeList;


/*
 * Union type.
 */
struct UnionType : public Type {
  /* Constituent types. */
  const TypeList& types;

  /* Constructs the type that is the union of the given types. */
  UnionType(const TypeList& types)
    : types(types) {
  }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const {
    for (TypeList::const_iterator i = types.begin(); i != types.end(); i++) {
      if ((*i)->subtype(t)) {
	return true;
      }
    }
    return false;
  }

protected:
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Simple type.
 */
struct SimpleType : public Type {
  /* The object type. */
  static const SimpleType& OBJECT_TYPE;

  /* Name of type. */
  const string name;
  /* Supertype */
  const Type& super_type;

  /* Constructs a simple type with the given name. */
  SimpleType(const string& name, const Type& super_type = OBJECT_TYPE)
    : name(name), super_type(name == "object" ? *this : super_type) {
  }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const {
    if (t.object()) {
      return true;
    } else {
      const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
      if (st != NULL) {
	return name == st->name || (!object() && super_type.subtype(t));
      } else {
	const UnionType& ut = dynamic_cast<const UnionType&>(t);
	for (TypeList::const_iterator i = ut.types.begin();
	     i != ut.types.end(); i++) {
	  if (subtype(**i)) {
	    return true;
	  }
	}
	return false;
      }
    }
  }

protected:
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const;
};

/* Checks if this type is object type. */
inline bool Type::object() const {
  return this == &SimpleType::OBJECT_TYPE;
}


/*
 * Table of types.
 */
typedef hash_map<string, const SimpleType*, hash<string>, equal_to<string>,
  container_alloc> TypeMap;


struct Term;
struct Variable;

/*
 * Variable substitution.
 */
typedef pair<const Variable*, const Term*> Substitution;


/*
 * List of substitutions.
 */
typedef vector<Substitution, container_alloc> SubstitutionList;


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
  virtual const Term& instantiation(unsigned int id) const {
    return *this;
  }

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Checks if this term is equivalent to the given term; two terms
     are equivalent if they both are the same name, or if they are
     variables (variable names only matter for equality and not for
     equivalence). */
  virtual bool equivalent(const Term& t) const {
    return name == t.name;
  }

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
  virtual const Variable& instantiation(unsigned int id) const;

  /* Returns this variable subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst) const {
    for (SubstitutionList::const_iterator i = subst.begin();
	 i != subst.end(); i++) {
      const Substitution& s = *i;
      if (*this == *s.first) {
	return *s.second;
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
  StepVar(const Variable& var, unsigned int id)
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
  unsigned int id_;
};


/* Returns an instantiation of this variable. */
inline const Variable& Variable::instantiation(unsigned int id) const {
  return *(new StepVar(*this, id));
}


/*
 * List of terms.
 */
struct TermList : public gc, public vector<const Term*, container_alloc> {
  /* Constructs an empty term list. */
  TermList() {
  }

  /* Returns an instantiation of this term list. */
  const TermList& instantiation(unsigned int id) const {
    TermList& terms = *(new TermList());
    for (const_iterator i = begin(); i != end(); i++) {
      terms.push_back(&(*i)->instantiation(id));
    }
    return terms;
  }

  /* Returns this term list subject to the given sunstitutions. */
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
 * Table of names.
 */
typedef hash_map<string, const Name*, hash<string>, equal_to<string>,
  container_alloc> NameMap;


/*
 * List of variables.
 */
struct VariableList : public gc,
		      public vector<const Variable*, container_alloc> {
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
  const VariableList& instantiation(unsigned int id) const {
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


/*
 * Abstract formula.
 */
struct Formula : public gc {
  /* Deletes this formula. */
  virtual ~Formula() {
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(unsigned int id) const = 0;

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
struct FormulaList : public gc,
		     public vector<const Formula*, container_alloc> {
  /* Constructs an empty formula list. */
  FormulaList() {
  }

  /* Constructs a formula list with a single formula. */
  FormulaList(const Formula* formula)
    : vector<const Formula*, container_alloc>(1, formula) {
  }

  /* Returns an instantiation of this formula list. */
  const FormulaList& instantiation(unsigned int id) const {
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
  virtual const AtomicFormula& instantiation(unsigned int id) const {
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
  virtual const Negation& instantiation(unsigned int id) const {
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
  virtual const Equality& instantiation(unsigned int id) const {
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
  virtual const Inequality& instantiation(unsigned int id) const {
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
  virtual const Conjunction& instantiation(unsigned int id) const {
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
  virtual const Disjunction& instantiation(unsigned int id) const {
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
  virtual const ExistsFormula& instantiation(unsigned int id) const {
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
  virtual const ForallFormula& instantiation(unsigned int id) const {
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
