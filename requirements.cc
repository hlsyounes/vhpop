/*
 * $Id: requirements.cc,v 1.1 2001-12-27 20:08:47 lorens Exp $
 */
#include "requirements.h"


/* ====================================================================== */
/* Requirements */

/* Constructs a default requirements object. */
Requirements::Requirements()
  : strips(true), typing(false), disjunctive_preconditions(false),
    equality(false), existential_preconditions(false),
    universal_preconditions(false), conditional_effects(false) {}


/* Enables quantified preconditions. */
void Requirements::quantified_preconditions() {
  existential_preconditions = true;
  universal_preconditions = true;
}


/* Enables ADL style actions. */
void Requirements::adl() {
  strips = true;
  typing = true;
  disjunctive_preconditions = true;
  equality = true;
  quantified_preconditions();
  conditional_effects = true;
}
