/*
 * $Id: support.cc,v 1.3 2001-10-06 22:55:26 lorens Exp $
 */

#include "support.h"


/* Prints this object on the given stream. */
void Unimplemented::print(ostream& os) const {
  os << "unimplemented: " << message;
}
