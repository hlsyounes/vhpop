/* -*-C++-*- */
/*
 * Expressions.
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
 * $Id: expressions.h,v 6.1 2003-12-05 23:19:02 lorens Exp $
 */
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <config.h>
#include "functions.h"
#include "terms.h"
#include <iostream>
#include <set>

struct Problem;


/* ====================================================================== */
/* Expression. */

struct ValueMap;

/*
 * An abstract expression.
 */
struct Expression {
  /* Register use of the given expression. */
  static void register_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_++;
    }
  }

  /* Unregister use of the given expression. */
  static void unregister_use(const Expression* e) {
    if (e != NULL) {
      e->ref_count_--;
      if (e->ref_count_ == 0) {
	delete e;
      }
    }
  }

  /* Deletes this expression. */
  virtual ~Expression();

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const = 0;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const = 0;

protected:
  /* Constructs an expression. */
  Expression();

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Value */

/*
 * A constant value.
 */
struct Value : public Expression {
  /* Constructs a constant value. */
  Value(float value) : value_(value) {}

  /* Returns the value of this expression. */
  float value() const { return value_; }

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Value& instantiation(const SubstitutionMap& subst,
				     const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* The value. */
  float value_;
};


/* ====================================================================== */
/* Application */

/*
 * A function application.
 */
struct Application : public Expression {
  /* Returns a function application with the given function and terms. */
  static const Application& make(Function function, const TermList& terms);

  /* Deletes this function application. */
  virtual ~Application();

  /* Returns the id for this application (zero if lifted). */
  size_t id() const { return id_; }

  /* Returns the function of this function application. */
  Function function() const { return function_; }

  /* Returns the number of terms of this function application. */
  size_t arity() const { return terms_.size(); }

  /* Returns the ith term of this function application. */
  Term term(size_t i) const { return terms_[i]; }

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns this application subject to the given substitution. */
  const Application& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

protected:
  /* Assigns an id to this application. */
  void assign_id(bool ground);

private:
  /* Less-than comparison function object for function applications. */
  struct ApplicationLess
    : public std::binary_function<const Application*, const Application*,
				  bool> {
    /* Comparison function. */
    bool operator()(const Application* a1, const Application* a2) const;
  };

  /* A table of function applications. */
  struct ApplicationTable : std::set<const Application*, ApplicationLess> {
  };

  /* Table of function applications. */
  static ApplicationTable applications;
  /* Next id for ground applications. */
  static size_t next_id;

  /* Unique id for ground applications (zero if lifted). */
  size_t id_;
  /* Function of this function application. */
  Function function_;
  /* Terms of this function application. */
  TermList terms_;

  /* Constructs a function application with the given function. */
  Application(Function function) : function_(function) {}

  /* Adds a term to this function application. */
  void add_term(Term term) { terms_.push_back(term); }
};

/*
 * Less than function object for application pointers.
 */
namespace std {
  struct less<const Application*>
    : public binary_function<const Application*, const Application*, bool> {
    /* Comparison function operator. */
    bool operator()(const Application* a1, const Application* a2) const {
      return a1->id() < a2->id();
    }
  };
}


/* ====================================================================== */
/* Computation */

/*
 * A computation expression.
 */
struct Computation : public Expression {
  /* Deletes this computation. */
  virtual ~Computation();

  /* Returns the first operand for this computation. */
  const Expression& operand1() const { return *operand1_; }

  /* Returns the second operand for this computation. */
  const Expression& operand2() const { return *operand2_; }

protected:
  /* Constructs a computation. */
  Computation(const Expression& operand1, const Expression& operand2);

private:
  /* The first operand for this computation. */
  const Expression* operand1_;
  /* The second operand for this computation. */
  const Expression* operand2_;
};


/* ====================================================================== */
/* Addition */

/*
 * An addition.
 */
struct Addition : public Computation {
  /* Returns an addition of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction.
 */
struct Subtraction : public Computation {
  /* Returns a subtraction of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication.
 */
struct Multiplication : public Computation {
  /* Returns a multiplication of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Division */

/*
 * A division.
 */
struct Division : public Computation {
  /* Returns a division of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Minimum */

/*
 * The minimum of two expressions.
 */
struct Minimum : public Computation {
  /* Returns the minimum of the two expressions. */
  static const Expression& make(const Expression& operand1,
				const Expression& operand2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs a minimum. */
  Minimum(const Expression& operand1, const Expression& operand2)
    : Computation(operand1, operand2) {}
};


/* ====================================================================== */
/* Maximum */

/*
 * The maximum of two expressions.
 */
struct Maximum : public Computation {
  /* Returns the maximum of the two expressions. */
  static const Expression& make(const Expression& operand1,
				const Expression& operand2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const;

private:
  /* Constructs a maximum. */
  Maximum(const Expression& operand1, const Expression& operand2)
    : Computation(operand1, operand2) {}
};


/* ====================================================================== */
/* ValueMap */

/*
 * Mapping from function applications to values.
 */
struct ValueMap : public std::map<const Application*, float> {
};


#endif /* EXPRESSIONS_H */
