/* -*-C++-*- */
/*
 * Types.
 *
 * $Id: types.h,v 1.5 2001-10-06 22:55:48 lorens Exp $
 */
#ifndef TYPES_H
#define TYPES_H

#include "support.h"


/*
 * Abstract type.
 */
struct Type : public EqualityComparable, public Printable, public gc {
  /* Checks if this type is the object type. */
  bool object() const;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const = 0;

protected:
  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const = 0;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const = 0;

  friend const Type& operator+(const Type& t1, const Type& t2);
  friend const Type& operator-(const Type& t1, const Type& t2);
};

/* Type union. */
inline const Type& operator+(const Type& t1, const Type& t2) {
  return t1.add(t2);
}

/* Type subtraction. */
inline const Type& operator-(const Type& t1, const Type& t2) {
  return t1.subtract(t2);
}


/*
 * Simple type.
 */
struct SimpleType : public Type {
  /* The object type. */
  static const SimpleType& OBJECT;

  /* Name of type. */
  const string name;
  /* Supertype */
  const Type& supertype;

  /* Constructs a simple type with the given name. */
  SimpleType(const string& name, const Type& supertype = OBJECT)
    : name(name), supertype(name == "object" ? *this : supertype) {
  }

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const;
};


/*
 * List of simple types.
 */
struct TypeList : public Vector<const SimpleType*> {
  /* Constructs an empty type list. */
  TypeList() {
  }

  /* Constructs a type list with a single type. */
  explicit TypeList(const SimpleType* type) {
    push_back(type);
  }
};

typedef TypeList::const_iterator TypeListIter;


/*
 * Table of simple types.
 */
struct TypeMap : HashMap<string, const SimpleType*> {
  /* Constructs an empty type table. */
  TypeMap() {
    (*this)[SimpleType::OBJECT.name] = &SimpleType::OBJECT;
  }
};

typedef TypeMap::const_iterator TypeMapIter;


/*
 * Union type.
 */
struct UnionType : public Type {
  /* Constituent types. */
  const TypeList& types;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const;

private:
  /* Constructs the type that is the union of the given types.
     N.B. Assumes type list is sorted. */
  UnionType(const TypeList& types)
    : types(types) {
  }

  friend const Type& SimpleType::add(const Type& t) const;
};

#endif /* TYPES_H */
