/*
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
 * $Id: expressions.cc,v 6.1 2003-12-05 23:19:07 lorens Exp $
 */
#include "expressions.h"
#include "problems.h"
#include "domains.h"
#include "debug.h"
#include <stdexcept>


/* ====================================================================== */
/* Expression */

/* Constructs an expression. */
Expression::Expression()
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_expressions++;
#endif
}


/* Deletes this expression. */
Expression::~Expression() {
#ifdef DEBUG_MEMORY
  deleted_expressions++;
#endif
}


/* ====================================================================== */
/* Value */

/* Returns the value of this expression in the given state. */
float Value::value(const ValueMap& values) const {
  return value_;
}


/* Returns an instantiation of this expression. */
const Value& Value::instantiation(const SubstitutionMap& subst,
				  const Problem& problem) const {
  return *this;
}


/* Prints this object on the given stream. */
void Value::print(std::ostream& os, const FunctionTable& functions,
		  const TermTable& terms) const {
  os << value_;
}


/* ====================================================================== */
/* Application */

/* Table of function applications. */
Application::ApplicationTable Application::applications;
/* Next id for ground applications. */
size_t Application::next_id = 1;


/* Comparison function. */
bool Application::ApplicationLess::operator()(const Application* a1,
					      const Application* a2) const {
  if (a1->function() < a2->function()) {
    return true;
  } else if (a1->function() > a2->function()) {
    return false;
  } else {
    for (size_t i = 0; i < a1->arity(); i++) {
      if (a1->term(i) < a2->term(i)) {
	return true;
      } else if (a1->term(i) > a2->term(i)) {
	return false;
      }
    }
    return false;
  }
}


/* Returns a function application with the given function and terms. */
const Application& Application::make(Function function,
				     const TermList& terms) {
  Application* application = new Application(function);
  bool ground = true;
  for (TermList::const_iterator ti = terms.begin(); ti != terms.end(); ti++) {
    application->add_term(*ti);
    if (ground && is_variable(*ti)) {
      ground = false;
    }
  }
  if (!ground) {
    application->assign_id(ground);
    return *application;
  } else {
    std::pair<ApplicationTable::const_iterator, bool> result =
      applications.insert(application);
    if (!result.second) {
      delete application;
      return **result.first;
    } else {
      application->assign_id(ground);
      return *application;
    }
  }
}


/* Deletes this function application. */
Application::~Application() {
  ApplicationTable::const_iterator ai = applications.find(this);
  if (*ai == this) {
    applications.erase(ai);
  }
}


/* Assigns an id to this application. */
void Application::assign_id(bool ground) {
  if (ground) {
    id_ = next_id++;
  } else {
    id_ = 0;
  }
}


/* Returns the value of this expression in the given state. */
float Application::value(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return (*vi).second;
  } else {
    throw std::logic_error("value of function application undefined");
  }
}


/* Returns this application subject to the given substitution. */
const Application&
Application::substitution(const SubstitutionMap& subst) const {
  TermList inst_terms;
  bool substituted = false;
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    SubstitutionMap::const_iterator si =
      is_variable(*ti) ? subst.find(*ti) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
    } else {
      inst_terms.push_back(*ti);
    }
  }
  if (substituted) {
    return make(function(), inst_terms);
  } else { 
    return *this;
  }
}


/* Returns an instantiation of this expression. */
const Expression& Application::instantiation(const SubstitutionMap& subst,
					     const Problem& problem) const {
  TermList inst_terms;
  bool substituted = false;
  size_t objects = 0;
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    SubstitutionMap::const_iterator si =
      is_variable(*ti) ? subst.find(*ti) : subst.end();
    if (si != subst.end()) {
      inst_terms.push_back((*si).second);
      substituted = true;
      objects++;
    } else {
      inst_terms.push_back(*ti);
      if (is_object(*ti)) {
	objects++;
      }
    }
  }
  if (substituted) {
    const Application& inst_appl = make(function(), inst_terms);
    if (problem.domain().functions().static_function(function())
	&& objects == inst_terms.size()) {
      ValueMap::const_iterator vi = problem.init_values().find(&inst_appl);
      if (vi != problem.init_values().end()) {
	return *new Value((*vi).second);
      } else {
	throw std::logic_error("value of static function application"
			       " undefined");
      }
    } else {
      return inst_appl;
    }
  } else { 
    return *this;
  }
}


/* Prints this object on the given stream. */
void Application::print(std::ostream& os, const FunctionTable& functions,
			const TermTable& terms) const {
  os << '(';
  functions.print_function(os, function());
  for (TermList::const_iterator ti = terms_.begin();
       ti != terms_.end(); ti++) {
    os << ' ';
    terms.print_term(os, *ti);
  }
  os << ')';
}


/* ====================================================================== */
/* Computation */

/* Constructs a computation. */
Computation::Computation(const Expression& operand1,
			 const Expression& operand2)
  : operand1_(&operand1), operand2_(&operand2) {
  register_use(operand1_);
  register_use(operand2_);
}


/* Deletes this computation. */
Computation::~Computation() {
  unregister_use(operand1_);
  unregister_use(operand2_);
}


/* ====================================================================== */
/* Addition */

/* Returns an addition of the two expressions. */
const Expression& Addition::make(const Expression& term1,
				 const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() + v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Addition(term1, term2);
}


/* Returns the value of this expression in the given state. */
float Addition::value(const ValueMap& values) const {
  return operand1().value(values) + operand2().value(values);
}


/* Returns an instantiation of this expression. */
const Expression& Addition::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Addition::print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const {
  os << "(+ ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Subtraction */

/* Returns a subtraction of the two expressions. */
const Expression& Subtraction::make(const Expression& term1,
				    const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() - v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Subtraction(term1, term2);
}


/* Returns the value of this expression in the given state. */
float Subtraction::value(const ValueMap& values) const {
  return operand1().value(values) - operand2().value(values);
}


/* Returns an instantiation of this expression. */
const Expression& Subtraction::instantiation(const SubstitutionMap& subst,
					     const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Subtraction::print(std::ostream& os, const FunctionTable& functions,
			const TermTable& terms) const {
  os << "(- ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Multiplication */

/* Returns a multiplication of the two expressions. */
const Expression& Multiplication::make(const Expression& factor1,
				       const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != NULL) {
      const Value& value = *new Value(v1->value() * v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Multiplication(factor1, factor2);
}


/* Returns the value of this expression in the given state. */
float Multiplication::value(const ValueMap& values) const {
  return operand1().value(values) * operand2().value(values);
}


/* Returns an instantiation of this expression. */
const Expression& Multiplication::instantiation(const SubstitutionMap& subst,
						const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Multiplication::print(std::ostream& os, const FunctionTable& functions,
			   const TermTable& terms) const {
  os << "(* ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Division */

/* Returns a division of the two expressions. */
const Expression& Division::make(const Expression& factor1,
				 const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != NULL) {
      if (v2->value() == 0) {
	throw std::logic_error("division by zero");
      }
      const Value& value = *new Value(v1->value() / v2->value());
      register_use(v1);
      register_use(v2);
      unregister_use(v1);
      unregister_use(v2);
      return value;
    }
  }
  return *new Division(factor1, factor2);
}


/* Returns the value of this expression in the given state. */
float Division::value(const ValueMap& values) const {
  float v2 = operand2().value(values);
  if (v2 == 0) {
    throw std::logic_error("division by zero");
  }
  return operand1().value(values) / v2;
}


/* Returns an instantiation of this expression. */
const Expression& Division::instantiation(const SubstitutionMap& subst,
					  const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Division::print(std::ostream& os, const FunctionTable& functions,
		     const TermTable& terms) const {
  os << "(/ ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Minimum */

/* Returns the minimum of the two expressions. */
const Expression& Minimum::make(const Expression& operand1,
				const Expression& operand2) {
  const Value* v1 = dynamic_cast<const Value*>(&operand1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&operand2);
    if (v2 != NULL) {
      if (v1->value() < v2->value()) {
	register_use(v2);
	unregister_use(v2);
	return *v1;
      } else {
	register_use(v1);
	unregister_use(v1);
	return *v2;
      }
    }
  }
  return *new Minimum(operand1, operand2);
}


/* Returns the value of this expression in the given state. */
float Minimum::value(const ValueMap& values) const {
  return std::min(operand1().value(values), operand2().value(values));
}


/* Returns an instantiation of this expression. */
const Expression& Minimum::instantiation(const SubstitutionMap& subst,
					 const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Minimum::print(std::ostream& os, const FunctionTable& functions,
		    const TermTable& terms) const {
  os << "(min ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}


/* ====================================================================== */
/* Maximum */

/* Returns the maximum of the two expressions. */
const Expression& Maximum::make(const Expression& operand1,
				const Expression& operand2) {
  const Value* v1 = dynamic_cast<const Value*>(&operand1);
  if (v1 != NULL) {
    const Value* v2 = dynamic_cast<const Value*>(&operand2);
    if (v2 != NULL) {
      if (v1->value() > v2->value()) {
	register_use(v2);
	unregister_use(v2);
	return *v1;
      } else {
	register_use(v1);
	unregister_use(v1);
	return *v2;
      }
    }
  }
  return *new Maximum(operand1, operand2);
}


/* Returns the value of this expression in the given state. */
float Maximum::value(const ValueMap& values) const {
  return std::max(operand1().value(values), operand2().value(values));
}


/* Returns an instantiation of this expression. */
const Expression& Maximum::instantiation(const SubstitutionMap& subst,
					 const Problem& problem) const {
  return make(operand1().instantiation(subst, problem),
	      operand2().instantiation(subst, problem));
}


/* Prints this object on the given stream. */
void Maximum::print(std::ostream& os, const FunctionTable& functions,
		    const TermTable& terms) const {
  os << "(max ";
  operand1().print(os, functions, terms);
  os << ' ';
  operand2().print(os, functions, terms);
  os << ")";
}
