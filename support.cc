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
 * $Id: support.cc,v 1.11 2002-09-23 04:06:40 lorens Exp $
 */

#include <cstdlib>
#include "support.h"


/* ====================================================================== */
/* Exception */

/* Constructs an exception with the given message. */
Exception::Exception(const string& message)
  : message(message) {}


/* Output operator for exceptions. */
ostream& operator<<(ostream& os, const Exception& e) {
  os << e.message;
  return os;
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
