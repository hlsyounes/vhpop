/*
 * $Id: support.cc,v 1.6 2001-12-23 16:26:19 lorens Exp $
 */

#include <cstdlib>
#include "support.h"


/* Output operator for printable objects. */
ostream& operator<<(ostream& os, const Printable& o) {
  o.print(os);
  return os;
}


/* Constructs an exception with the given message. */
Exception::Exception(const string& message)
  : message(message) {}


/* Prints this object on the given stream. */
void Exception::print(ostream& os) const {
  os << message;
}


/* Constructs an unimplemented exception. */
Unimplemented::Unimplemented(const string& message)
  : Exception(message) {}


/* Generates a random number in the interval [0,1]. */
double rand01() {
  return rand()/double(RAND_MAX);
}


/* Generates a random number in the interval [0,1). */
double rand01ex() {
  return rand()/(RAND_MAX + 1.0);
}
