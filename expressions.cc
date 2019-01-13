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

#include "expressions.h"
#include <stdexcept>


/* ====================================================================== */
/* Expression */

/*Output operator for expressions. */
std::ostream& operator<<(std::ostream& os, const Expression& e) {
  e.print(os);
  return os;
}


/* ====================================================================== */
/* Value */

/* Returns the value of this expression in the given state. */
float Value::value(const ValueMap& values) const {
  return value_;
}


/* Returns an instantiation of this expression. */
const Value& Value::instantiation(const std::map<Variable, Term>& subst,
                                  const ValueMap& values) const {
  return *this;
}

/* Prints this object on the given stream. */
void Value::print(std::ostream& os) const {
  os << value_;
}


/* ====================================================================== */
/* Fluent */

/* Table of fluents. */
Fluent::FluentTable Fluent::fluents;
/* Next id for ground fluents. */
size_t Fluent::next_id = 1;


/* Comparison function. */
bool Fluent::FluentLess::operator()(const Fluent* f1, const Fluent* f2) const {
  if (f1->function() < f2->function()) {
    return true;
  } else if (f2->function() < f1->function()) {
    return false;
  } else {
    return f1->terms() < f2->terms();
  }
}


/* Returns a fluent with the given function and terms. */
const Fluent& Fluent::make(const Function& function,
                           const std::vector<Term>& terms) {
  Fluent* fluent = new Fluent(function);
  bool ground = true;
  for (std::vector<Term>::const_iterator ti = terms.begin(); ti != terms.end();
       ti++) {
    fluent->add_term(*ti);
    if (ground && (*ti).variable()) {
      ground = false;
    }
  }
  if (!ground) {
    fluent->assign_id(ground);
    return *fluent;
  } else {
    std::pair<FluentTable::const_iterator, bool> result =
      fluents.insert(fluent);
    if (!result.second) {
      delete fluent;
      return **result.first;
    } else {
      fluent->assign_id(ground);
      return *fluent;
    }
  }
}

/* Deletes this fluent. */
Fluent::~Fluent() {
  FluentTable::const_iterator fi = fluents.find(this);
  if (*fi == this) {
    fluents.erase(fi);
  }
}


/* Assigns an id to this fluent. */
void Fluent::assign_id(bool ground) {
  if (ground) {
    id_ = next_id++;
  } else {
    id_ = 0;
  }
}


/* Returns the value of this expression in the given state. */
float Fluent::value(const ValueMap& values) const {
  ValueMap::const_iterator vi = values.find(this);
  if (vi != values.end()) {
    return (*vi).second;
  } else {
    throw std::logic_error("value of fluent is undefined");
  }
}


/* Returns this fluent subject to the given substitution. */
const Fluent& Fluent::substitution(
    const std::map<Variable, Term>& subst) const {
  std::vector<Term> inst_terms;
  bool substituted = false;
  for (std::vector<Term>::const_iterator ti = terms().begin();
       ti != terms().end(); ti++) {
    std::map<Variable, Term>::const_iterator si =
        (*ti).variable() ? subst.find((*ti).as_variable()) : subst.end();
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
const Expression& Fluent::instantiation(const std::map<Variable, Term>& subst,
                                        const ValueMap& values) const {
  if (terms().empty()) {
    if (FunctionTable::static_function(function())) {
      ValueMap::const_iterator vi = values.find(this);
      if (vi != values.end()) {
	return *new Value((*vi).second);
      } else {
	throw std::logic_error("value of static fluent is undefined");
      }
    } else {
      return *this;
    }
  } else {
    std::vector<Term> inst_terms;
    bool substituted = false;
    size_t objects = 0;
    for (std::vector<Term>::const_iterator ti = terms().begin();
         ti != terms().end(); ti++) {
      std::map<Variable, Term>::const_iterator si =
          (*ti).variable() ? subst.find((*ti).as_variable()) : subst.end();
      if (si != subst.end()) {
	inst_terms.push_back((*si).second);
	substituted = true;
	objects++;
      } else {
	inst_terms.push_back(*ti);
	if ((*ti).object()) {
	  objects++;
	}
      }
    }
    if (substituted) {
      const Fluent& inst_fluent = make(function(), inst_terms);
      if (FunctionTable::static_function(function())
	  && objects == inst_terms.size()) {
	ValueMap::const_iterator vi = values.find(&inst_fluent);
	if (vi != values.end()) {
	  return *new Value((*vi).second);
	} else {
	  throw std::logic_error("value of static fluent is undefined");
	}
      } else {
	return inst_fluent;
      }
    } else { 
      return *this;
    }
  }
}

/* Prints this object on the given stream. */
void Fluent::print(std::ostream& os) const {
  os << '(' << function();
  for (std::vector<Term>::const_iterator ti = terms().begin();
       ti != terms().end(); ti++) {
    os << ' ' << *ti;
  }
  os << ')';
}


/* ====================================================================== */
/* Computation */

/* Constructs a computation. */
Computation::Computation(const Expression& operand1,
			 const Expression& operand2)
  : operand1_(&operand1), operand2_(&operand2) {
  ref(operand1_);
  ref(operand2_);
}


/* Deletes this computation. */
Computation::~Computation() {
  destructive_deref(operand1_);
  destructive_deref(operand2_);
}


/* ====================================================================== */
/* Addition */

/* Returns an addition of the two expressions. */
const Expression& Addition::make(const Expression& term1,
				 const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != 0) {
      const Value& value = *new Value(v1->value() + v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
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
const Expression& Addition::instantiation(const std::map<Variable, Term>& subst,
                                          const ValueMap& values) const {
  const Expression& inst_term1 = operand1().instantiation(subst, values);
  const Expression& inst_term2 = operand2().instantiation(subst, values);
  if (&inst_term1 == &operand1() && &inst_term2 == &operand2()) {
    return *this;
  } else {
    return make(inst_term1, inst_term2);
  }
}

/* Prints this object on the given stream. */
void Addition::print(std::ostream& os) const {
  os << "(+ " << operand1() << ' ' << operand2() << ")";
}


/* ====================================================================== */
/* Subtraction */

/* Returns a subtraction of the two expressions. */
const Expression& Subtraction::make(const Expression& term1,
				    const Expression& term2) {
  const Value* v1 = dynamic_cast<const Value*>(&term1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&term2);
    if (v2 != 0) {
      const Value& value = *new Value(v1->value() - v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
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
const Expression& Subtraction::instantiation(
    const std::map<Variable, Term>& subst, const ValueMap& values) const {
  const Expression& inst_term1 = operand1().instantiation(subst, values);
  const Expression& inst_term2 = operand2().instantiation(subst, values);
  if (&inst_term1 == &operand1() && &inst_term2 == &operand2()) {
    return *this;
  } else {
    return make(inst_term1, inst_term2);
  }
}

/* Prints this object on the given stream. */
void Subtraction::print(std::ostream& os) const {
  os << "(- " << operand1() << ' ' << operand2() << ")";
}


/* ====================================================================== */
/* Multiplication */

/* Returns a multiplication of the two expressions. */
const Expression& Multiplication::make(const Expression& factor1,
				       const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != 0) {
      const Value& value = *new Value(v1->value() * v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
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
const Expression& Multiplication::instantiation(
    const std::map<Variable, Term>& subst, const ValueMap& values) const {
  const Expression& inst_factor1 = operand1().instantiation(subst, values);
  const Expression& inst_factor2 = operand2().instantiation(subst, values);
  if (&inst_factor1 == &operand1() && &inst_factor2 == &operand2()) {
    return *this;
  } else {
    return make(inst_factor1, inst_factor2);
  }
}

/* Prints this object on the given stream. */
void Multiplication::print(std::ostream& os) const {
  os << "(* " << operand1() << ' ' << operand2() << ")";
}


/* ====================================================================== */
/* Division */

/* Returns a division of the two expressions. */
const Expression& Division::make(const Expression& factor1,
				 const Expression& factor2) {
  const Value* v1 = dynamic_cast<const Value*>(&factor1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&factor2);
    if (v2 != 0) {
      if (v2->value() == 0) {
	throw std::logic_error("division by zero");
      }
      const Value& value = *new Value(v1->value() / v2->value());
      ref(v1);
      ref(v2);
      destructive_deref(v1);
      destructive_deref(v2);
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
const Expression& Division::instantiation(const std::map<Variable, Term>& subst,
                                          const ValueMap& values) const {
  const Expression& inst_factor1 = operand1().instantiation(subst, values);
  const Expression& inst_factor2 = operand2().instantiation(subst, values);
  if (&inst_factor1 == &operand1() && &inst_factor2 == &operand2()) {
    return *this;
  } else {
    return make(inst_factor1, inst_factor2);
  }
}

/* Prints this object on the given stream. */
void Division::print(std::ostream& os) const {
  os << "(/ " << operand1() << ' ' << operand2() << ")";
}


/* ====================================================================== */
/* Minimum */

/* Returns the minimum of the two expressions. */
const Expression& Minimum::make(const Expression& operand1,
				const Expression& operand2) {
  const Value* v1 = dynamic_cast<const Value*>(&operand1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&operand2);
    if (v2 != 0) {
      if (v1->value() < v2->value()) {
	ref(v2);
	destructive_deref(v2);
	return *v1;
      } else {
	ref(v1);
	destructive_deref(v1);
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
const Expression& Minimum::instantiation(const std::map<Variable, Term>& subst,
                                         const ValueMap& values) const {
  const Expression& inst_oper1 = operand1().instantiation(subst, values);
  const Expression& inst_oper2 = operand2().instantiation(subst, values);
  if (&inst_oper1 == &operand1() && &inst_oper2 == &operand2()) {
    return *this;
  } else {
    return make(inst_oper1, inst_oper2);
  }
}

/* Prints this object on the given stream. */
void Minimum::print(std::ostream& os) const {
  os << "(min " << operand1() << ' ' << operand2() << ")";
}


/* ====================================================================== */
/* Maximum */

/* Returns the maximum of the two expressions. */
const Expression& Maximum::make(const Expression& operand1,
				const Expression& operand2) {
  const Value* v1 = dynamic_cast<const Value*>(&operand1);
  if (v1 != 0) {
    const Value* v2 = dynamic_cast<const Value*>(&operand2);
    if (v2 != 0) {
      if (v1->value() > v2->value()) {
	ref(v2);
	destructive_deref(v2);
	return *v1;
      } else {
	ref(v1);
	destructive_deref(v1);
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
const Expression& Maximum::instantiation(const std::map<Variable, Term>& subst,
                                         const ValueMap& values) const {
  const Expression& inst_oper1 = operand1().instantiation(subst, values);
  const Expression& inst_oper2 = operand2().instantiation(subst, values);
  if (&inst_oper1 == &operand1() && &inst_oper2 == &operand2()) {
    return *this;
  } else {
    return make(inst_oper1, inst_oper2);
  }
}

/* Prints this object on the given stream. */
void Maximum::print(std::ostream& os) const {
  os << "(max " << operand1() << ' ' << operand2() << ")";
}
