/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.7 2001-10-11 21:14:11 lorens Exp $
 */
#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include <string>
#include <vector>
#include <hash_map>
#include <gc/gc_cpp.h>
#include <gc/new_gc_alloc.h>


/*
 * Allocator to use with all STL container classes.
 */
typedef single_client_traceable_alloc container_alloc;


/*
 * A vector using a traceable allocator.
 */
template<typename T>
struct Vector : public vector<T, container_alloc>, public gc {
};


/*
 * A hash map using a traceable allocator.
 */
template<typename K, typename T,
	 typename H = hash<K>, typename E = equal_to<K> >
struct HashMap : public hash_map<K, T, H, E, container_alloc>, public gc {
};


/*
 * A equality comparable object.
 */
struct EqualityComparable {
  virtual ~EqualityComparable() {}

protected:
  /* Checks if this object equals the given object. */
  virtual bool equals(const EqualityComparable& o) const = 0;

  friend bool operator==(const EqualityComparable& o1,
			 const EqualityComparable& o2);
};

/* Equality operator for equality comparable objects. */
inline bool operator==(const EqualityComparable& o1,
		       const EqualityComparable& o2) {
  return o1.equals(o2);
}

/* Inequality operator for equality comparable objects. */
inline bool operator!=(const EqualityComparable& o1,
		       const EqualityComparable& o2) {
  return !(o1 == o2);
}

/*
 * Equality function object for equality comparable object pointers.
 */
struct equal_to<const EqualityComparable*>
  : public binary_function<const EqualityComparable*,
			   const EqualityComparable*, bool> {
  bool operator()(const EqualityComparable* o1,
		  const EqualityComparable* o2) const {
    return *o1 == *o2;
  }
};


/*
 * A hashable object.
 */
struct Hashable : public EqualityComparable {
protected:
  /* Returns the hash value of this object. */
  virtual size_t hash_value() const = 0;

  friend struct hash<Hashable>;
  friend struct hash<const Hashable*>;
};

/*
 * Hash function object for hashable objects.
 */
struct hash<Hashable> {
  size_t operator()(const Hashable& o) const {
    return o.hash_value();
  }
};

/*
 * Hash function object for hashable object pointers.
 */
struct hash<const Hashable*> {
  size_t operator()(const Hashable* o) const {
    return o->hash_value();
  }
};


/*
 * A printable object.
 */
struct Printable {
  virtual ~Printable() {}

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const = 0;

  friend ostream& operator<<(ostream& os, const Printable& o);
};

/* Output operator for printable objects. */
inline ostream& operator<<(ostream& os, const Printable& o) {
  o.print(os);
  return os;
}


/*
 * Run-time exception.
 */
struct Exception : public Printable {
  /* Constructs an exception with the given message. */
  Exception(const string& message)
    : message(message) {
  }

protected:
  /* Prints this object on the given stream. */
  void print(ostream& os) const;

private:
  /* Message. */
  string message;
};


/*
 * Exception thrown for unimplemented features.
 */
struct Unimplemented : public Exception {
  /* Constructs an unimplemented exception. */
  Unimplemented(const string& message)
    : Exception(message) {
  }
};


/*
 * Hash function object for strings.
 */
struct hash<string> {
  /* Hash function for strings. */
  size_t operator()(const string& s) const {
    return hash<char*>()(s.c_str());
  }
};

#endif /* SUPPORT_H */
