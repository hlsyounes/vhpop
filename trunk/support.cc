/*
 * $Id: support.cc,v 1.1 2001-07-29 18:10:53 lorens Exp $
 */

#include "support.h"


void Unimplemented::print(ostream& os) const {
  os << "unimplemented: " << message;
}
