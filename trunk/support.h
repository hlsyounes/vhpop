/* -*-C++-*- */
/*
 * Auxiliary types and functions.
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
 * $Id: support.h,v 3.9 2002-09-22 23:20:53 lorens Exp $
 */
#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include <string>
#if HAVE_HASH_MAP
#include <hash_map>
#include <hash_set>
#else
#include <ext/hash_map>
#include <ext/hash_set>
#endif
#include "debug.h"

using namespace std;


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
 * Hash function object for strings.
 */
namespace std {
struct hash<string> {
  /* Hash function for strings. */
  size_t operator()(const string& s) const {
    return hash<char*>()(s.c_str());
  }
};
}


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
