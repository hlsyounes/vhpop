/* -*-C++-*- */
/*
 * Types.
 *
 * $Id: types.h,v 1.2 2001-08-10 04:41:03 lorens Exp $
 */
#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include "support.h"


/*
 * Abstract type.
 */
struct Type : public gc {
  /* Deletes this type. */
  virtual ~Type() {
  }

  /* Checks if this type is the object type. */
  bool object() const;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const = 0;

protected:
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const = 0;

  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const = 0;

  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const = 0;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const = 0;

  friend ostream& operator<<(ostream& os, const Type& t);
  friend bool operator==(const Type& t1, const Type& t2);
  friend const Type& operator+(const Type& t1, const Type& t2);
  friend const Type& operator-(const Type& t1, const Type& t2);
};

/* Output operator for types. */
inline ostream& operator<<(ostream& os, const Type& t) {
  t.print(os);
  return os;
}

/* Equality operator for types. */
inline bool operator==(const Type& t1, const Type& t2) {
  return t1.equals(t2);
}

/* Inequality operator for types. */
inline bool operator!=(const Type& t1, const Type& t2) {
  return !(t1 == t2);
}

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
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const;

  /* Returns the union of this type and the given type. */
  virtual const Type& add(const Type& t) const;

  /* Removes the given type from this type. */
  virtual const Type& subtract(const Type& t) const;
};


/*
 * List of simple types.
 */
struct TypeList : public gc, vector<const SimpleType*, container_alloc> {
  /* Constructs an empty type list. */
  TypeList() {
  }

  /* Constructs a type list containing n copies of t. */
  TypeList(size_t n, const SimpleType* t)
    : vector<const SimpleType*, container_alloc>(n, t) {
  }
};


/*
 * Table of simple types.
 */
struct TypeMap : public gc,
		 hash_map<string, const SimpleType*, hash<string>,
		 equal_to<string>, container_alloc> {
  /* Constructs an empty type table. */
  TypeMap() {
    (*this)[SimpleType::OBJECT.name] = &SimpleType::OBJECT;
  }
};


/*
 * Union type.
 */
struct UnionType : public Type {
  /* Constituent types. */
  const TypeList& types;

  /* Checks if this type is a subtype of the given type. */
  virtual bool subtype(const Type& t) const;

protected:
  /* Prints this type on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this type equals the given type. */
  virtual bool equals(const Type& t) const;

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
