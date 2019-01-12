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

#include "domains.h"
#include "bindings.h"
#include <iostream>


/* ====================================================================== */
/* Domain */

/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Returns a const_iterator pointing to the first domain. */
Domain::DomainMap::const_iterator Domain::begin() {
  return domains.begin();
}


/* Returns a const_iterator pointing beyond the last domain. */
Domain::DomainMap::const_iterator Domain::end() {
  return domains.end();
}


/* Returns the domain with the given name, or NULL it is undefined. */
const Domain* Domain::find(const std::string& name) {
  DomainMap::const_iterator di = domains.find(name);
  return (di != domains.end()) ? (*di).second : NULL;
}


/* Removes all defined domains. */
void Domain::clear() {
  DomainMap::const_iterator di = begin();
  while (di != end()) {
    delete (*di).second;
    di = begin();
  }
  domains.clear();
}


/* Constructs an empty domain with the given name. */
Domain::Domain(const std::string& name)
  : name_(name), total_time_(functions_.add_function("total-time")) {
  const Domain* d = find(name);
  if (d != NULL) {
    delete d;
  }
  domains[name] = this;
  FunctionTable::make_dynamic(total_time_);
}


/* Deletes a domain. */
Domain::~Domain() {
  domains.erase(name());
  for (std::map<std::string, const ActionSchema*>::const_iterator ai =
           actions_.begin();
       ai != actions_.end(); ai++) {
    delete (*ai).second;
  }
}


/* Adds an action to this domain. */
void Domain::add_action(const ActionSchema& action) {
  actions_.insert(make_pair(action.name(), &action));
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const std::string& name) const {
  std::map<std::string, const ActionSchema*>::const_iterator ai =
      actions_.find(name);
  return (ai != actions_.end()) ? (*ai).second : NULL;
}


/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d) {
  os << "name: " << d.name();
  os << std::endl << "types:" << d.types();
  os << std::endl << "constants:" << d.terms();
  os << std::endl << "predicates:" << d.predicates();
  os << std::endl << "functions:" << d.functions();
  os << std::endl << "actions:";
  for (std::map<std::string, const ActionSchema*>::const_iterator ai =
           d.actions_.begin();
       ai != d.actions_.end(); ai++) {
    os << std::endl;
    (*ai).second->print(os);
  }
  return os;
}
