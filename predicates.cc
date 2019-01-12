// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include "predicates.h"


/* ====================================================================== */
/* Predicate */

/* Output operator for predicates. */
std::ostream& operator<<(std::ostream& os, const Predicate& p) {
  os << PredicateTable::names_[p.index_];
  return os;
}


/* ====================================================================== */
/* PredicateTable */

/* Predicate names. */
std::vector<std::string> PredicateTable::names_;
/* Predicate parameters. */
std::vector<std::vector<Type>> PredicateTable::parameters_;
/* Static predicates. */
PredicateSet PredicateTable::static_predicates_;


/* Adds a parameter with the given type to the given predicate. */
void PredicateTable::add_parameter(const Predicate& predicate,
				   const Type& type) {
  parameters_[predicate.index_].push_back(type);
}


/* Returns the name of the given predicate. */
const std::string& PredicateTable::name(const Predicate& predicate) {
  return names_[predicate.index_];
}


/* Returns the parameter types of the given predicate. */
const std::vector<Type>& PredicateTable::parameters(
    const Predicate& predicate) {
  return parameters_[predicate.index_];
}

/* Makes the given predicate dynamic. */
void PredicateTable::make_dynamic(const Predicate& predicate) {
  static_predicates_.erase(predicate);
}


/* Tests if the given predicate is static. */
bool PredicateTable::static_predicate(const Predicate& predicate) {
  return static_predicates_.find(predicate) != static_predicates_.end();
}


/* Adds a predicate with the given name to this table and returns
   the predicate. */
const Predicate& PredicateTable::add_predicate(const std::string& name) {
  std::pair<std::map<std::string, Predicate>::const_iterator, bool> pi =
    predicates_.insert(std::make_pair(name, Predicate(names_.size())));
  const Predicate& predicate = (*pi.first).second;
  names_.push_back(name);
  parameters_.push_back(std::vector<Type>());
  static_predicates_.insert(predicate);
  return predicate;
}
 
 
/* Returns a pointer to the predicate with the given name, or 0 if
   no predicate with the given name exists. */
const Predicate*
PredicateTable::find_predicate(const std::string& name) const {
  std::map<std::string, Predicate>::const_iterator pi = predicates_.find(name);
  if (pi != predicates_.end()) {
    return &(*pi).second;
  } else {
    return 0;
  }
}


/* Output operator for predicate tables. */
std::ostream& operator<<(std::ostream& os, const PredicateTable& t) {
  for (std::map<std::string, Predicate>::const_iterator pi =
	 t.predicates_.begin();
       pi != t.predicates_.end(); pi++) {
    const Predicate& p = (*pi).second;
    os << std::endl << "  (" << p;
    const std::vector<Type>& types = PredicateTable::parameters(p);
    for (std::vector<Type>::const_iterator ti = types.begin();
         ti != types.end(); ti++) {
      os << " ?v - " << *ti;
    }
    os << ")";
    if (PredicateTable::static_predicate(p)) {
      os << " <static>";
    }
  }
  return os;
}
