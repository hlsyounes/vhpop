/*
 * Copyright (C) 2002 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: support.cc,v 1.10 2002-06-28 20:14:07 lorens Exp $
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
