/*
 * $Id: support.cc,v 1.4 2001-10-11 21:14:17 lorens Exp $
 */

#include "support.h"


/* Prints this object on the given stream. */
void Exception::print(ostream& os) const {
  os << message;
}
