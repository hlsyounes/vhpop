/* -*-C++-*- */
/*
 * Types.
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
 * $Id: types.h,v 3.4 2002-03-18 09:41:29 lorens Exp $
 */
#ifndef TYPES_H
#define TYPES_H

#include "support.h"


/*
 * Abstract type.
 */
struct Type : public EqualityComparable, public Printable {
  /* Checks if this type is the object type. */
  bool object() const;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const = 0;
};


/*
 * Simple type.
 */
struct SimpleType : public LessThanComparable, public Type {
  /* The object type. */
  static const SimpleType OBJECT;

  /* Constructs a simple type with the given name. */
  explicit SimpleType(const string& name, const Type& supertype = OBJECT);

  /* Returns the name of this type. */
  const string& name() const { return name_; }

  /* Returns the supertype of this type. */
  const Type& supertype() const { return *supertype_; }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Name of type. */
  string name_;
  /* Supertype */
  const Type* supertype_;
};


/*
 * Table of simple types.
 */
struct TypeMap : hash_map<string, const SimpleType*> {
};

/* Iterator for type tables. */
typedef TypeMap::const_iterator TypeMapIter;


/* Set of simple types. */
struct TypeSet
  : public set<const SimpleType*, less<const LessThanComparable*> > {
};

/* Iterator for type lists. */
typedef TypeSet::const_iterator TypeSetIter;


/*
 * Union type.
 */
struct UnionType : public Type {
  /* Returns the canonical form of the given union type. */
  static const Type& simplify(const UnionType& t);

  /* Returns the union of two types. */
  static const Type& add(const Type& t1, const Type& t2);

  /* Constructs a singleton union type. */
  explicit UnionType(const SimpleType& type);

  /* Returns the constituent types of this union type. */
  const TypeSet& types() const { return types_; }

  /* Adds the given simple type to this union. */
  void add(const SimpleType& t);

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Constituent types. */
  TypeSet types_;
};


#endif /* TYPES_H */
