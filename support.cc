/*
 * $Id: support.cc,v 1.5 2001-12-22 18:49:49 lorens Exp $
 */

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
