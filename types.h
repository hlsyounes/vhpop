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
 * $Id: types.h,v 3.12 2002-11-05 04:43:16 lorens Exp $
 */
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <set>


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
  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend bool operator==(const Type& t1, const Type& t2);
  friend std::ostream& operator<<(std::ostream& os, const Type& t);
};

/* Equality operator for types. */
inline bool operator==(const Type& t1, const Type& t2) {
  return t1.equals(t2);
}

/* Inequality operator for types. */
inline bool operator!=(const Type& t1, const Type& t2) {
  return !(t1 == t2);
}

/* Output operator for types. */
std::ostream& operator<<(std::ostream& os, const Type& t);


/* ====================================================================== */
/* SimpleType */

/*
 * Simple type.
 */
struct SimpleType : public Type {
  /* Constructs a simple type with the given name. */
  explicit SimpleType(const std::string& name, const Type& supertype = OBJECT);

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
  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const;

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
/* TypeSet */

/* Set of simple types. */
struct TypeSet : public std::set<const SimpleType*> {
};

/* Iterator for type lists. */
typedef TypeSet::const_iterator TypeSetIter;


/* ====================================================================== */
/* UnionType */
/*
 * Union type.
 */
struct UnionType : public Type {
  /* Returns the canonical form of the given union type. */
  static const Type& simplify(const UnionType& t);

  /* Returns the union of two types. */
  static const Type& add(const Type& t1, const Type& t2);

  /* Constructs an empty union type. */
  UnionType();

  /* Constructs a singleton union type. */
  explicit UnionType(const SimpleType& type);

  /* Adds the given simple type to this union. */
  void add(const SimpleType& t);

  /* Returns the constituent types of this union type. */
  const TypeSet& types() const { return types_; }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constituent types. */
  TypeSet types_;
};


#endif /* TYPES_H */
