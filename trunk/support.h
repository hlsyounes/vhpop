/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.5 2001-09-28 17:55:34 lorens Exp $
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
 * A printable object.
 */
struct Printable : public gc {
  virtual ~Printable() {
  }

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
 * Abstract exception.
 */
struct Exception : public Printable {
};


/*
 * Exception thrown for unimplemented features.
 */
struct Unimplemented : public Exception {
  /* Constructs an unimplemented exception. */
  Unimplemented(const string& message)
    : message(message) {
  }

protected:
  /* Prints this exception on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Message. */
  string message;
};


/*
 * Hash function object for strings.
 */
struct hash<string> : public gc {
  /* Hash function for strings. */
  size_t operator()(const string& s) const {
    return hash<char*>()(s.c_str());
  }
};

#endif /* SUPPORT_H */
