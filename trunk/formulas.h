/* -*-C++-*- */
/*
 * Formulas.
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
 * $Id: formulas.h,v 4.9 2002-11-05 04:42:23 lorens Exp $
 */
#ifndef FORMULAS_H
#define FORMULAS_H

#include "support.h"
#include <map>

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

struct Type;
struct Predicate;
struct Domain;
struct Problem;
struct Bindings;
struct HeuristicValue;
struct PlanningGraph;


struct Term;
struct Variable;


/* ====================================================================== */
/* Substitution */

/*
 * Variable substitution.
 */
struct Substitution {
  /* Constructs a substitution. */
  Substitution(const Variable& var, const Term& term);

  /* Constructs a substitution with assigned step ids. */
  Substitution(const Variable& var, size_t var_id,
	       const Term& term, size_t term_id);

  /* Returns the variable to get substituted. */
  const Variable& var() const { return *var_; }

  /* Returns the step id of the variable. */
  size_t var_id() const { return var_id_; }

  /* Returns the term to substitute with. */
  const Term& term() const { return *term_; }

  /* Returns the step id of the term. */
  size_t term_id() const { return term_id_; }

private:
  /* Variable to get substituted. */
  const Variable* var_;
  /* Step id of variable. */
  size_t var_id_;
  /* Term to substitute with. */
  const Term* term_;
  /* Step id of term. */
  size_t term_id_;
};


/* ====================================================================== */
/* SubstitutionList */

/*
 * List of substitutions.
 */
struct SubstitutionList : public vector<Substitution> {
};

/* Substitution list iterator. */
typedef SubstitutionList::const_iterator SubstListIter;


/* ====================================================================== */
/* Term */

/*
 * Abstract term.
 */
struct Term {
  /* Deletes this term. */
  virtual ~Term();

  /* Adds the given type to the type of this term. */
  void add_type(const Type& type);

  /* Returns the name of this term. */
  const string& name() const { return name_; }

  /* Returns the type of this term. */
  const Type& type() const { return *type_; }

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst,
				   size_t step_id) const = 0;

  /* Prints this term on the given stream with the given bindings. */
  void print(ostream& os, size_t step_id, const Bindings& bindings) const;

protected:
  /* Constructs an abstract term with the given name. */
  Term(const string& name, const Type& type);

private:
  /* Name of term. */
  string name_;
  /* Type of term. */
  const Type* type_;
};

/*
 * Hash function object for term pointers.
 */
namespace __gnu_cxx {
struct hash<const Term*> {
  size_t operator()(const Term* t) const {
    return size_t(t);
  }
};
}

/* Equality operator for terms. */
inline bool operator==(const Term& t1, const Term& t2) {
  return &t1 == &t2;
}

/* Inequality operator for terms. */
inline bool operator!=(const Term& t1, const Term& t2) {
  return &t1 != &t2;
}

/* Output operator for terms. */
ostream& operator<<(ostream& os, const Term& t);


/* ====================================================================== */
/* Name */

/*
 * Name.
 */
struct Name : public Term {
  /* Constructs a name. */
  Name(const string& name, const Type& type);

  /* Returns this term subject to the given substitutions. */
  virtual const Name& substitution(const SubstitutionList& subst,
				   size_t step_id) const;
};


/* ====================================================================== */
/* Variable */

/*
 * Variable.
 */
struct Variable : public Term {
  /* Constructs a variable with the given name and type. */
  Variable(const string& name, const Type& type);

  /* Returns this term subject to the given substitutions. */
  virtual const Term& substitution(const SubstitutionList& subst,
				   size_t step_id) const;
};


/* ====================================================================== */
/* TermList */

/*
 * List of terms.
 */
struct TermList : public vector<const Term*> {
  /* Returns this term list subject to the given substitutions. */
  const TermList& substitution(const SubstitutionList& subst,
			       size_t step_id) const;
};

/* Term list iterator. */
typedef TermList::const_iterator TermListIter;


/* ====================================================================== */
/* NameList */

/*
 * List of names.
 */
struct NameList : public vector<const Name*> {
  /* An empty name list. */
  static const NameList EMPTY;
};

/* Name list iterator. */
typedef NameList::const_iterator NameListIter;


/* ====================================================================== */
/* NameMap */

/*
 * Table of names.
 */
struct NameMap : public map<string, Name*> {
};

/* Name table iterator. */
typedef NameMap::const_iterator NameMapIter;


/* ====================================================================== */
/* VariableList */

/*
 * List of variables.
 */
struct VariableList : public vector<const Variable*> {
  /* An empty variable list. */
  static const VariableList EMPTY;
};

/* Variable list iterator. */
typedef VariableList::const_iterator VarListIter;


/* ====================================================================== */
/* Formula */

struct Literal;

/*
 * Abstract formula.
 */
struct Formula {
  /* Possible temporal annotations for formulas. */
  typedef enum { AT_START, AT_END, OVER_ALL } FormulaTime;

  /* The true formula. */
  static const Formula& TRUE;
  /* The false formula. */
  static const Formula& FALSE;

  /* Checks if this formula is a tautology. */
  bool tautology() const;

  /* Checks if this formula is a contradiction. */
  bool contradiction() const;

  /* Checks if this formula is either a tautology or contradiction. */
  bool constant() const;

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const = 0;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const = 0;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id,
			       const Bindings* b = NULL) const = 0;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b = NULL) const = 0;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const = 0;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const = 0;

  friend const Formula& operator!(const Formula& f);
  friend ostream& operator<<(ostream& os, const Formula& f);
};

/* Negation operator for formulas. */
inline const Formula& operator!(const Formula& f) {
  return f.negation();
}

/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2);

/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2);

/* Output operator for formulas. */
ostream& operator<<(ostream& os, const Formula& f);


/* ====================================================================== */
/* FormulaList */

/*
 * List of formulas.
 */
struct FormulaList : public vector<const Formula*> {
  /* Constructs an empty formula list. */
  FormulaList();

  /* Constructs a formula list with a single formula. */
  explicit FormulaList(const Formula* formula);

  /* Returns the negation of this formula list. */
  const FormulaList& negation() const;
};

/* A formula list const iterator. */
typedef FormulaList::const_iterator FormulaListIter;


/* ====================================================================== */
/* Constant */

/*
 * A formula with a constant truth value.
 */
struct Constant : public Formula {
  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Constant& instantiation(const SubstitutionList& subst,
					const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Constant& substitution(const SubstitutionList& subst,
				       size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* Constant representing true. */
  static const Constant TRUE_;
  /* Constant representing false. */
  static const Constant FALSE_;

  /* Value of this constant. */
  bool value_;

  /* Constructs a constant formula. */
  Constant(bool value);

  friend struct Formula;
};


/* ====================================================================== */
/* Literal */

/*
 * An abstract literal.
 */
struct Literal : public Formula {
  /* Returns the temporal annotation for this formula. */
  FormulaTime when() const { return when_; }

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const = 0;

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const = 0;

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

protected:
  /* Constructs a literal. */
  explicit Literal(FormulaTime when);

  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const = 0;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const = 0;

private:
  /* Temporal annotation for this formula. */
  FormulaTime when_;

  friend bool operator==(const Literal& l1, const Literal& l2);
  friend struct hash<const Literal*>;
};

/* Equality operator for literals. */
inline bool operator==(const Literal& l1, const Literal& l2) {
  return l1.equals(l2);
}

/* Inequality operator for literals. */
inline bool operator!=(const Literal& l1, const Literal& l2) {
  return !(l1 == l2);
}

/*
 * Equality function object for literal pointers.
 */
namespace std {
struct equal_to<const Literal*>
  : public binary_function<const Literal*, const Literal*, bool> {
  bool operator()(const Literal* l1, const Literal* l2) const {
    return *l1 == *l2;
  }
};
}

/*
 * Hash function object for literal pointers.
 */
namespace __gnu_cxx {
struct hash<const Literal*> {
  size_t operator()(const Literal* l) const {
    return l->hash_value();
  }
};
}


/* ====================================================================== */
/* Atom */

/*
 * An atom.
 */
struct Atom : public Literal {
  /* Constructs an atomic formula. */
  Atom(const Predicate& predicate, const TermList& terms, FormulaTime when);

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const { return *predicate_; }

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const { return *terms_; }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Atom& substitution(const SubstitutionList& subst,
				   size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;

private:
  /* Predicate of this atom. */
  const Predicate* predicate_;
  /* Terms of this atom. */
  const TermList* terms_;
};


/* ====================================================================== */
/* Negation */

/*
 * A negated atom.
 */
struct Negation : public Literal {
  /* Constructs a negated atom. */
  explicit Negation(const Atom& atom);

  /* Returns the the negated atom. */
  const Atom& atom() const { return *atom_; }

  /* Returns the predicate of this literal. */
  virtual const Predicate& predicate() const { return atom().predicate(); }

  /* Returns the terms of this literal. */
  virtual const TermList& terms() const { return atom().terms(); }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Negation& substitution(const SubstitutionList& subst,
				       size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const Literal& o) const;

  /* Returns the hash value of this object. */
  virtual size_t hash_value() const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Literal& negation() const;

private:
  /* The negated atom. */
  const Atom* atom_;
};


/* ====================================================================== */
/* BindingLiteral */

/*
 * A binding literal.
 */
struct BindingLiteral : public Formula {
  /* Returns the first term of binding literal. */
  const Term& term1() const { return *term1_; }

  /* Returns the step id of the first term, or the given id if no step
     id has been assigned. */
  size_t step_id1(size_t def_id) const { return (id1_ != 0) ? id1_ : def_id; }

  /* Returns the second term of binding literal. */
  const Term& term2() const { return *term2_; }

  /* Returns the step id of the second term, or the given id if no
     step id has been assigned. */
  size_t step_id2(size_t def_id) const { return (id2_ != 0) ? id2_ : def_id; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

protected:
  /* Constructs a binding literal. */
  BindingLiteral(const Term& term1, size_t id1, const Term& term2, size_t id2);

private:
  /* First term of binding literal. */
  const Term* term1_;
  /* Step id of first term, or zero if unassigned. */
  size_t id1_;
  /* Second term of binding literal. */
  const Term* term2_;
  /* Step id of second term, or zero if unassigned. */
  size_t id2_;
};


/* ====================================================================== */
/* Equality */

/*
 * Equality formula.
 */
struct Equality : public BindingLiteral {
  /* Constructs an equality. */
  Equality(const Term& term1, const Term& term2);

  /* Constructs an equality with assigned step ids. */
  Equality(const Term& term1, size_t id1, const Term& term2, size_t id2);

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const BindingLiteral& negation() const;
};


/* ====================================================================== */
/* Inequality */

/*
 * Inequality formula.
 */
struct Inequality : public BindingLiteral {
  /* Constructs an inequality. */
  Inequality(const Term& term1, const Term& term2);

  /* Constructs an inequality with assigned step ids. */
  Inequality(const Term& term1, size_t id1, const Term& term2, size_t id2);

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const BindingLiteral& negation() const;
};


/* ====================================================================== */
/* Conjunction */

/*
 * Conjunction.
 */
struct Conjunction : public Formula {
  /* Returns the conjuncts. */
  const FormulaList& conjuncts() const { return *conjuncts_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* The conjuncts. */
  const FormulaList* conjuncts_;

  /* Constructs a conjunction. */
  Conjunction(const FormulaList& conjuncts);

  friend const Formula& operator&&(const Formula& f1, const Formula& f2);
};


/* ====================================================================== */
/* Disjunction */

/*
 * Disjunction.
 */
struct Disjunction : public Formula {
  /* Returns the disjuncts. */
  const FormulaList& disjuncts() const { return *disjuncts_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitution. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const Formula& negation() const;

private:
  /* The disjuncts. */
  const FormulaList* disjuncts_;

  /* Constructs a disjunction. */
  Disjunction(const FormulaList& disjuncts);

  friend const Formula& operator||(const Formula& f1, const Formula& f2);
};


/* ====================================================================== */
/* QuantifiedFormula */

/*
 * Abstract quantified formula.
 */
struct QuantifiedFormula : public Formula {
  /* Returns the quanitfied variables. */
  const VariableList& parameters() const { return *parameters_; }

  /* Returns the quantified formula. */
  const Formula& body() const { return *body_; }

  /* Returns a formula that separates the given literal from anything
     definitely asserted by this formula. */
  virtual const Formula& separate(const Literal& literal) const;

protected:
  /* Constructs a quantified formula. */
  QuantifiedFormula(const VariableList& parameters, const Formula& body);

private:
  /* Quanitfied variables. */
  const VariableList* parameters_;
  /* The quantified formula. */
  const Formula* body_;
};


/* ====================================================================== */
/* ExistsFormula */

/*
 * Existentially quantified formula.
 */
struct ExistsFormula : public QuantifiedFormula {
  /* Constructs an existentially quantified formula. */
  ExistsFormula(const VariableList& parameters, const Formula& body);

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/* ====================================================================== */
/* ForallFormula */

/*
 * Universally quantified formula.
 */
struct ForallFormula : public QuantifiedFormula {
  /* Constructs a universally quantified formula. */
  ForallFormula(const VariableList& parameters, const Formula& body);

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const;

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst,
				      size_t step_id) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       size_t step_id, const Bindings* b) const;

  /* Returns the heuristic value of this formula. */
  virtual void heuristic_value(HeuristicValue& h, HeuristicValue& hs,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const;

  /* Prints this formula on the given stream with the given bindings. */
  virtual void print(ostream& os, size_t step_id,
		     const Bindings& bindings) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the negation of this formula. */
  virtual const QuantifiedFormula& negation() const;
};


/* ====================================================================== */
/* AtomList */

/*
 * List of atoms.
 */
struct AtomList : public vector<const Atom*> {
  /* An empty atom list. */
  static const AtomList EMPTY;

  /* Constructs an empty atom list. */
  AtomList();

  /* Constructs an atom list with a single atom. */
  explicit AtomList(const Atom* atom);

  /* Returns this atom list subject to the given substitutions. */
  const AtomList& substitution(const SubstitutionList& subst,
			       size_t step_id) const;
};

/* Atom list iterator. */
typedef AtomList::const_iterator AtomListIter;


/* ====================================================================== */
/* NegationList */

/*
 * List of negated atoms.
 */
struct NegationList : public vector<const Negation*> {
  /* An empty negation list. */
  static const NegationList EMPTY;

  /* Constructs an empty negation list. */
  NegationList();

  /* Constructs a negation list with a single negated atom. */
  NegationList(const Atom* atom);

  /* Returns this negation list subject to the given substitutions. */
  const NegationList& substitution(const SubstitutionList& subst,
				   size_t step_id) const;
};

/* Negation list iterator */
typedef NegationList::const_iterator NegationListIter;


#endif /* FORMULAS_H */
