/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.2 2001-07-29 18:12:02 lorens Exp $
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
  const string message;

  Unimplemented(const string& message)
    : message(message) {
  }
private:
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
    return (tail == NULL) ? 1 : 1 + tail->size();
  }

  /* Checks if this chain contains the given element. */
  bool contains(const T& h) const {
    if (h == head) {
      return true;
    } else if (tail != NULL) {
      return tail->contains(h);
    } else {
      return false;
    }
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
