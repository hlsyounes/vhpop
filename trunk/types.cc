/*
 * Copyright (C) 2002 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: types.cc,v 4.4 2002-11-05 04:43:13 lorens Exp $
 */
#include "types.h"
#include <iostream>


/*
 * A subtype binary predicate.
 */
struct Subtype : public std::binary_function<const Type*, const Type*, bool> {
  /* Checks if the first type is a subtype of the second type. */
  bool operator()(const Type* t1, const Type* t2) const {
    return t1->subtype(*t2);
  }
};


/* ====================================================================== */
/* Type */

/* The object type. */
const Type& Type::OBJECT = SimpleType::OBJECT_;


/* Checks if this type is the object type. */
bool Type::object() const {
  return this == &Type::OBJECT;
}


/* Output operator for types. */
std::ostream& operator<<(std::ostream& os, const Type& t) {
  t.print(os);
  return os;
}


/* ====================================================================== */
/* SimpleType */

/* The object type. */
const SimpleType SimpleType::OBJECT_("object");


/* Constructs a simple type with the given name. */
SimpleType::SimpleType(const std::string& name, const Type& supertype)
  : name_(name),
    supertype_(name == "object" ? (const Type*) this : &supertype) {}


/* Attemts to add the given supertype to this type.  Returns false
   if the intended supertype is a subtype of this type. */
bool SimpleType::add_supertype(const Type& supertype) {
  if (supertype.subtype(*this)) {
    return false;
  } else {
    const Type& new_supertype = UnionType::add(*supertype_, supertype);
    const UnionType* ut = dynamic_cast<const UnionType*>(supertype_);
    if (ut != NULL) {
      delete ut;
    }
    supertype_ = &new_supertype;
    return true;
  }
}


/* Checks if this type is a subtype of the given type. */
bool SimpleType::subtype(const Type& t) const {
  if (this == &t || t.object()) {
    return true;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      return !object() && supertype().subtype(t);
    } else {
      const UnionType& ut = dynamic_cast<const UnionType&>(t);
      return find_if(ut.types().begin(), ut.types().end(),
		     bind1st(Subtype(), this)) != ut.types().end();
    }
  }
}


/* Checks if this type equals the given type. */
bool SimpleType::equals(const Type& t) const {
  return this == &t;
}


/* Prints this object on the given stream. */
void SimpleType::print(std::ostream& os) const {
  os << name();
}


/* ====================================================================== */
/* UnionType */

/* Returns the canonical form of the given union type. */
const Type& UnionType::simplify(const UnionType& t) {
  const Type* canonical_type;
  if (t.types().empty()) {
    canonical_type = &Type::OBJECT;
    delete &t;
  } else if (t.types().size() == 1) {
    canonical_type = *t.types().begin();
    delete &t;
  } else {
    canonical_type = &t;
  }
  return *canonical_type;
}


/* Returns the union of two types. */
const Type& UnionType::add(const Type& t1, const Type& t2) {
  UnionType* t;
  const SimpleType* st1 = dynamic_cast<const SimpleType*>(&t1);
  if (st1 != NULL) {
    t = new UnionType(*st1);
  } else {
    const UnionType& ut1 = dynamic_cast<const UnionType&>(t1);
    t = new UnionType(ut1);
  }
  const SimpleType* st2 = dynamic_cast<const SimpleType*>(&t2);
  if (st2 != NULL) {
    t->add(*st2);
  } else {
    const UnionType& ut2 = dynamic_cast<const UnionType&>(t2);
    for (TypeSetIter ti = ut2.types().begin(); ti != ut2.types().end(); ti++) {
      t->add(**ti);
    }
  }
  return simplify(*t);
}


/* Constructs an empty union type. */
UnionType::UnionType() {
}


/* Constructs a singleton union type. */
UnionType::UnionType(const SimpleType& type) {
  types_.insert(&type);
}


/* Adds the given simple type to this union. */
void UnionType::add(const SimpleType& t) {
  if (!subtype(t)) {
    TypeSetIter ti =
      find_if(types().begin(), types().end(), bind1st(Subtype(), &t));
    while (ti != types().end()) {
      types_.erase(ti);
      ti = find_if(types().begin(), types().end(), bind1st(Subtype(), &t));
    }
    types_.insert(&t);
  }
}


/* Checks if this type is a subtype of the given type. */
bool UnionType::subtype(const Type& t) const {
  return (find_if(types().begin(), types().end(), bind2nd(Subtype(), &t))
	  != types().end());
}


/* Checks if this type equals the given type. */
bool UnionType::equals(const Type& t) const {
  const UnionType* ut = dynamic_cast<const UnionType*>(&t);
  return (ut != NULL && types().size() == ut->types().size()
	  && equal(types().begin(), types().end(), ut->types().begin()));
}


/* Prints this object on the given stream. */
void UnionType::print(std::ostream& os) const {
  os << "(either";
  for (TypeSetIter ti = types().begin(); ti != types().end(); ti++) {
    os << ' ' << **ti;
  }
  os << ")";
}
