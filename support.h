/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.4 2001-09-04 18:19:14 lorens Exp $
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

#endif /* SUPPORT_H */
