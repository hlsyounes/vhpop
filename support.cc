/*
 * $Id: support.cc,v 1.2 2001-08-10 04:08:42 lorens Exp $
 */

#include "support.h"


/* Prints this exception on the given stream. */
void Unimplemented::print(ostream& os) const {
  os << "unimplemented: " << message;
}
