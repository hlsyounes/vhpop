/*
 * $Id: support.cc,v 1.7 2001-12-27 19:53:37 lorens Exp $
 */

#include <cstdlib>
#include "support.h"


/* ====================================================================== */
/* Printable */

/* Output operator for printable objects. */
ostream& operator<<(ostream& os, const Printable& o) {
  o.print(os);
  return os;
}


/* ====================================================================== */
/* Exception */

/* Constructs an exception with the given message. */
Exception::Exception(const string& message)
  : message(message) {}


/* Prints this object on the given stream. */
void Exception::print(ostream& os) const {
  os << message;
}


/* ====================================================================== */
/* Unimplemented */

/* Constructs an unimplemented exception. */
Unimplemented::Unimplemented(const string& message)
  : Exception(message) {}


/* ====================================================================== */
/* Random number generation */

/* Generates a random number in the interval [0,1]. */
double rand01() {
  return rand()/double(RAND_MAX);
}


/* Generates a random number in the interval [0,1). */
double rand01ex() {
  return rand()/(RAND_MAX + 1.0);
}
