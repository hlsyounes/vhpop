/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
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
 * $Id: support.h,v 3.7 2002-06-28 20:13:44 lorens Exp $
 */
#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include <string>
#include <hash_map>
#include "debug.h"


/*
 * A collectible object.
 */
struct Collectible {
  /* Register use of this object. */
  static void register_use(const Collectible* c) {
    if (c != NULL) {
      c->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const Collectible* c) {
    if (c != NULL) {
      c->ref_count_--;
      if (c->ref_count_ == 0) {
	delete c;
      }
    }
  }

  /* Deletes this collectible object. */
  virtual ~Collectible() {
#ifdef DEBUG_MEMORY
    deleted_collectibles++;
#endif
  }

protected:
  /* Constructs a collectible object. */
  Collectible()
    : ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

  /* Constructs a copy of a collectible object. */
  Collectible(const Collectible& collectible)
    : ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* Checks if the given sequence contains the given value. */
template<typename I, typename T>
inline bool member(I first, I last, T value) {
  return find(first, last, value) != last;
}


/* Checks if the given sequence contains a value satisfying the given
   predicate. */
template<typename I, typename P>
inline bool member_if(I first, I last, P pred) {
  return find_if(first, last, pred) != last;
}


/*
 * A hash multimap.
 */
template<typename K, typename T,
	 typename H = hash<K>, typename E = equal_to<K> >
struct HashMultimap : public hash_multimap<K, T, H, E> {
  typedef typename hash_multimap<K, T, H, E>::const_iterator const_iterator;

  /* Finds the given element. */
  const_iterator find(const pair<K, T>& x) const {
    pair<const_iterator, const_iterator> bounds = equal_range(x.first);
    for (const_iterator i = bounds.first; i != bounds.second; i++) {
      if ((*i).second == x.second) {
	return i;
      }
    }
    return end();
  }
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
 * A less than comparable object.
 */
struct LessThanComparable {
  virtual ~LessThanComparable() {}

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const = 0;

  friend bool operator<(const LessThanComparable& o1,
			const LessThanComparable& o2);
};

/* Less than operator for less than comparable objects. */
inline bool operator<(const LessThanComparable& o1,
		      const LessThanComparable& o2) {
  return o1.less(o2);
}


/* Greater than operator for less than comparable objects. */
inline bool operator>(const LessThanComparable& o1,
		      const LessThanComparable& o2) {
  return o2 < o1;
}

/*
 * Less than function object for less than comparable object pointers.
 */
struct less<const LessThanComparable*>
  : public binary_function<const LessThanComparable*,
			   const LessThanComparable*, bool> {
  bool operator()(const LessThanComparable* o1,
		  const LessThanComparable* o2) const {
    return *o1 < *o2;
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
 * Hash function object for strings.
 */
struct hash<string> {
  /* Hash function for strings. */
  size_t operator()(const string& s) const {
    return hash<char*>()(s.c_str());
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
ostream& operator<<(ostream& os, const Printable& o);


/* An ostream iterator outputting a space before each object. */
template <typename T>
struct pre_ostream_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;

  /* Constructs an ostream iterator. */
  pre_ostream_iterator(ostream& s)
    : stream_(&s) {}

  /* Assigns to this ostream iterator. */
  pre_ostream_iterator<T>& operator=(const T* v) { 
    *stream_ << ' ' << *v;
    return *this;
  }

  /* Assigns to this ostream iterator. */
  pre_ostream_iterator<T>& operator=(const T& v) { 
    *stream_ << ' ' << v;
    return *this;
  }

  pre_ostream_iterator<T>& operator*() { return *this; }
  pre_ostream_iterator<T>& operator++() { return *this; } 
  pre_ostream_iterator<T>& operator++(int) { return *this; } 

private:
  /* The stream associated with this iterator. */
  ostream* stream_;
};


/*
 * Run-time exception.
 */
struct Exception : public Printable {
  /* Constructs an exception with the given message. */
  Exception(const string& message);

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
  Unimplemented(const string& message);
};


/* Generates a random number in the interval [0,1]. */
double rand01();

/* Generates a random number in the interval [0,1). */
double rand01ex();


#endif /* SUPPORT_H */