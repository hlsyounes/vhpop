/* -*-C++-*- */
/*
 * Types.
 *
 * $Id: types.h,v 1.8 2001-12-25 18:15:40 lorens Exp $
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
const Type& operator+(const Type& t1, const Type& t2);

/* Type subtraction. */
const Type& operator-(const Type& t1, const Type& t2);


/*
 * Simple type.
 */
struct SimpleType : public LessThanComparable, public Type {
  /* The object type. */
  static const SimpleType& OBJECT;

  /* Name of type. */
  const string name;
  /* Supertype */
  const Type& supertype;

  /* Constructs a simple type with the given name. */
  explicit SimpleType(const string& name, const Type& supertype = OBJECT);

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const;

  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const;
};


struct TypeList;

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
  explicit UnionType(const TypeList& types);

  friend const Type& SimpleType::add(const Type& t) const;
};


/*
 * List of simple types.
 */
struct TypeList : public Vector<const SimpleType*> {
  /* Constructs an empty type list. */
  TypeList();

  /* Constructs a type list with a single type. */
  explicit TypeList(const SimpleType* type);
};

/* Iterator for type lists. */
typedef TypeList::const_iterator TypeListIter;


/*
 * Table of simple types.
 */
struct TypeMap : HashMap<string, const SimpleType*> {
};

/* Iterator for type tables. */
typedef TypeMap::const_iterator TypeMapIter;


#endif /* TYPES_H */
