/* -*-C++-*- */
/*
 * Auxiliary types and functions.
 *
 * $Id: support.h,v 1.1 2001-05-03 15:23:43 lorens Exp $
 */
#ifndef SUPPORT_H
#define SUPPORT_H

#include <hash_map>
#include <string>
#include <ctype.h>
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
};


/*
 * Hash function for string objects.
 */
struct hash<string> {
  size_t operator()(const string& s) const {
    return h(s.c_str());
  }

private:
  hash<char*> h;
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


/*
 * Converts a string to lowercase.
 */
inline string tolower(const char* s) {
  string result;
  for (const char* p = s; *p != '\0'; p++) {
    result += tolower(*p);
  }
  return result;
}

/*
 * Converts an integer to a string.
 */
inline string tostring(unsigned int n) {
  string result;
  while (n > 0) {
    result = char(n % 10 + '0') + result;
    n /= 10;
  }
  if (result.length() == 0) {
    return "0";
  } else {
    return result;
  }
}

#endif /* SUPPORT_H */
