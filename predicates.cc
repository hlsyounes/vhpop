/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: predicates.cc,v 6.1 2003-03-23 10:51:53 lorens Exp $
 */
#include "predicates.h"


/* ====================================================================== */
/* PredicateTable */

/* Adds a predicate with the given name to this table and returns
   the predicate. */
Predicate PredicateTable::add_predicate(const std::string& name) {
  Predicate predicate = size();
  names_.push_back(name);
  predicates_.insert(std::make_pair(name, predicate));
  parameters_.push_back(TypeList());
  static_predicates_.insert(predicate);
  return predicate;
}


/* Returns the predicate with the given name.  If no predicate with
   the given name exists, false is returned in the second part of
   the result. */
std::pair<Predicate, bool>
PredicateTable::find_predicate(const std::string& name) const {
  std::map<std::string, Predicate>::const_iterator pi = predicates_.find(name);
  if (pi != predicates_.end()) {
    return std::make_pair((*pi).second, true);
  } else {
    return std::make_pair(0, false);
  }
}
