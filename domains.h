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
//
// Domain descriptions.

#ifndef DOMAINS_H
#define DOMAINS_H

#include <map>

#include "actions.h"
#include "functions.h"
#include "predicates.h"
#include "requirements.h"
#include "terms.h"
#include "types.h"

/* ====================================================================== */
/* Domain */

/*
 * Domain definition.
 */
struct Domain {
  /* Table of domain definitions. */
  struct DomainMap : public std::map<std::string, const Domain*> {
  };

  /* Requirements for this domain. */
  Requirements requirements;

  /* Returns a const_iterator pointing to the first domain. */
  static DomainMap::const_iterator begin();

  /* Returns a const_iterator pointing beyond the last domain. */
  static DomainMap::const_iterator end();

  /* Returns the domain with the given name, or NULL it is undefined. */
  static const Domain* find(const std::string& name);

  /* Removes all defined domains. */
  static void clear();

  /* Constructs an empty domain with the given name. */
  Domain(const std::string& name);

  /* Deletes a domain. */
  ~Domain();

  /* Returns the name of this domain. */
  const std::string& name() const { return name_; }

  /* Domain actions. */
  const std::map<std::string, const ActionSchema*>& actions() const {
    return actions_;
  }

  /* Returns the type table of this domain. */
  TypeTable& types() { return types_; }

  /* Returns the type table of this domain. */
  const TypeTable& types() const { return types_; }

  /* Returns the predicate table of this domain. */
  PredicateTable& predicates() { return predicates_; }

  /* Returns the predicate table of this domain. */
  const PredicateTable& predicates() const { return predicates_; }

  /* Returns the function table of this domain. */
  FunctionTable& functions() { return functions_; }

  /* Returns the function table of this domain. */
  const FunctionTable& functions() const { return functions_; }

  /* Returns the `total-time' function. */
  const Function& total_time() const { return total_time_; }

  /* Returns the term table of this domain. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this domain. */
  const TermTable& terms() const { return terms_; }

  /* Adds an action to this domain. */
  void add_action(const ActionSchema& action);

  /* Returns the action schema with the given name, or NULL if it is
     undefined. */
  const ActionSchema* find_action(const std::string& name) const;

private:
  /* Table of all defined domains. */
  static DomainMap domains;

  /* Name of this domain. */
  std::string name_;
  /* Domain types. */
  TypeTable types_;
  /* Domain predicates. */
  PredicateTable predicates_;
  /* Domain functions. */
  FunctionTable functions_;
  /* The `total-time' function. */
  Function total_time_;
  /* Domain terms. */
  TermTable terms_;
  /* Domain action schemas. */
  std::map<std::string, const ActionSchema*> actions_;

  friend std::ostream& operator<<(std::ostream& os, const Domain& d);
};

/* Output operator for domains. */
std::ostream& operator<<(std::ostream& os, const Domain& d);


#endif /* DOMAINS_H */
