/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.3 2001-08-10 04:08:18 lorens Exp $
 */
#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include <string>
#include <hash_map>
#include <gc/gc_cpp.h>
#include <gc/new_gc_alloc.h>


/*
 * Allocator to use with all STL container classes.
 */
typedef single_client_traceable_alloc container_alloc;


/*
 * Exception thrown for unimplemented features.
 */
struct Unimplemented {
  /* Message. */
  const string message;

  /* Constructs an unimplemented exception. */
  Unimplemented(const string& message)
    : message(message) {
  }
private:
  /* Prints this exception on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Unimplemented& ex);
};

/* Output operator for unimplemented exception. */
inline ostream& operator<<(ostream& os, const Unimplemented& ex) {
  ex.print(os);
  return os;
}


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
 * Template chain class.
 */
template<class T>
struct Chain : public gc {
  T head;
  const Chain<T>* tail;

  /* Constructs a chain with the given head and tail. */
  Chain<T>(const T& h, const Chain<T>* t)
    : head(h), tail(t) {
  }

  /* Returns the size of this chain. */
  size_t size() const {
    size_t result = 0;
    for (const Chain<T>* ci = this; ci != NULL; ci = ci->tail) {
      result++;
    }
    return result;
  }

  /* Checks if this chain contains the given element. */
  bool contains(const T& h) const {
    for (const Chain<T>* ci = this; ci != NULL; ci = ci->tail) {
      if (h == ci->head) {
	return true;
      }
    }
    return false;
  }

  /* Returns a chain with the first occurance of the given element removed. */
  const Chain<T>* remove(const T& h) const {
    if (h == head) {
      return tail;
    } else if (tail != NULL) {
      return new Chain<T>(head, tail->remove(h));
    } else {
      return this;
    }
  }
};

#endif /* SUPPORT_H */
