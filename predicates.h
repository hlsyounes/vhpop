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
// PDDL predicates.

#ifndef PREDICATES_H_
#define PREDICATES_H_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"

// A predicate.
class Predicate {
 public:
  // Constructs a predicate.
  explicit Predicate(int index) : index_(index) {}

 private:
  // Predicate index.
  int index_;

  friend bool operator==(const Predicate& p1, const Predicate& p2);
  friend bool operator<(const Predicate& p1, const Predicate& p2);
  friend std::ostream& operator<<(std::ostream& os, const Predicate& p);
  friend struct PredicateTable;
};

// Equality operator for predicates.
inline bool operator==(const Predicate& p1, const Predicate& p2) {
  return p1.index_ == p2.index_;
}

// Inequality operator for predicates.
inline bool operator!=(const Predicate& p1, const Predicate& p2) {
  return !(p1 == p2);
}

// Less-than operator for predicates.
inline bool operator<(const Predicate& p1, const Predicate& p2) {
  return p1.index_ < p2.index_;
}

// Output operator for predicates.
std::ostream& operator<<(std::ostream& os, const Predicate& p);

// Predicate table.
class PredicateTable {
 public:
  // Adds a parameter with the given type to the given predicate.
  static void add_parameter(const Predicate& predicate, const Type& type);

  // Returns the name of the given predicate.
  static const std::string& name(const Predicate& predicate);

  // Returns the parameter types of the given predicate.
  static const std::vector<Type>& parameters(const Predicate& predicate);

  // Makes the given predicate dynamic.
  static void make_dynamic(const Predicate& predicate);

  // Tests if the given predicate is static.
  static bool static_predicate(const Predicate& predicate);

  // Adds a predicate with the given name to this table and returns the
  // predicate.
  const Predicate& add_predicate(const std::string& name);

  // Returns a pointer to the predicate with the given name, or 0 if no
  // predicate with the given name exists.
  const Predicate* find_predicate(const std::string& name) const;

 private:
  // Predicate names.
  static std::vector<std::string> names_;
  // Predicate parameters.
  static std::vector<std::vector<Type>> parameters_;
  // Static predicates.
  static std::set<Predicate> static_predicates_;

  // Mapping of predicate names to predicates.
  std::map<std::string, Predicate> predicates_;

  friend std::ostream& operator<<(std::ostream& os, const PredicateTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Predicate& p);
};

// Output operator for predicate tables.
std::ostream& operator<<(std::ostream& os, const PredicateTable& t);

#endif  // PREDICATES_H_
