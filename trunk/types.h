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
 * $Id: types.h,v 3.16 2002-12-18 22:59:26 lorens Exp $
 */
#ifndef TYPES_H
#define TYPES_H

#include <config.h>
#include <string>
#include <vector>
#include <map>


/* ====================================================================== */
/* Type */

/*
 * Abstract type.
 */
struct Type {
  /* The object type. */
  static const Type& OBJECT;

  /* Checks if this type is the object type. */
  bool object() const;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Type& t);
};

/* Output operator for types. */
std::ostream& operator<<(std::ostream& os, const Type& t);


/* ====================================================================== */
/* SimpleType */

/*
 * Simple type.
 */
struct SimpleType : public Type {
  /* Constructs a simple type with the given name. */
  SimpleType(const std::string& name, const Type& supertype);

  /* Attemts to add the given supertype to this type.  Returns false
     if the intended supertype is a subtype of this type. */
  bool add_supertype(const Type& supertype);

  /* Returns the name of this simple type. */
  const std::string& name() const { return name_; }

  /* Returns the supertype of this simple type. */
  const Type& supertype() const { return *supertype_; }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The object type. */
  static const SimpleType OBJECT_;

  /* Name of type. */
  std::string name_;
  /* Supertype */
  const Type* supertype_;

  friend struct Type;
};


/* ====================================================================== */
/* TypeList */

/* List of types. */
struct TypeList : std::vector<const Type*> {
};

/* Iterator for type list. */
typedef TypeList::const_iterator TypeListIter;


/* ====================================================================== */
/* SimpleTypeList */

/* List of simple types. */
struct SimpleTypeList : public std::vector<const SimpleType*> {
};

/* Iterator for simple type list. */
typedef SimpleTypeList::const_iterator SimpleTypeListIter;


/* ====================================================================== */
/* UnionType */
/*
 * Union type.
 */
struct UnionType : public Type {
  /* Simplifies the given union type. */
  static const Type& simplify(const UnionType& t);

  /* Returns the union of two types. */
  static const Type& add(const Type& t1, const Type& t2);

  /* Constructs an empty union type. */
  UnionType();

  /* Constructs a singleton union type. */
  explicit UnionType(const SimpleType& t);

  /* Adds the given simple type to this union type. */
  void add(const SimpleType& t);

  /* Returns the constituent types of this union type. */
  const SimpleTypeList& types() const { return types_; }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constituent types. */
  SimpleTypeList types_;
};


/* ====================================================================== */
/* TypeMap */

/*
 * Table of simple types.
 */
struct TypeMap : std::map<std::string, SimpleType*> {
};

/* Iterator for type tables. */
typedef TypeMap::const_iterator TypeMapIter;


#endif /* TYPES_H */
