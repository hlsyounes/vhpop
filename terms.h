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
// PDDL terms.

#ifndef TERMS_H_
#define TERMS_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "types.h"

class Term;

// An object.
class Object {
 public:
  // Constructs an object.
  explicit Object(int index) : index_(index) {}

  // Converts this object to a term.
  operator Term() const;

 private:
  // Object index.
  int index_;
};

// A variable.
class Variable {
 public:
  // Constructs a variable.
  explicit Variable(int index) : index_(index) {}

  // Converts this variable to a term.
  operator Term() const;

 private:
  // Variable index.
  int index_;
};

// A term.
class Term {
 public:
  // Constructs a term.
  explicit Term(int index) : index_(index) {}

  // Tests if this term is an object.
  bool object() const { return index_ >= 0; }

  // Tests if this term is a variable.
  bool variable() const { return index_ < 0; }

  // Converts this term to an object.  Fails if the term is not an object.
  Object as_object() const;

  // Converts this term to a variable.  Fails if the term is not a variable.
  Variable as_variable() const;

 private:
  // Term index.
  int index_;

  friend bool operator==(const Term& t1, const Term& t2);
  friend bool operator<(const Term& t1, const Term& t2);
  friend std::ostream& operator<<(std::ostream& os, const Term& t);
  friend struct TermTable;
};

inline Object::operator Term() const { return Term(index_); }

inline Variable::operator Term() const { return Term(index_); }

// Equality operator for terms.
inline bool operator==(const Term& t1, const Term& t2) {
  return t1.index_ == t2.index_;
}

// Inequality operator for terms.
inline bool operator!=(const Term& t1, const Term& t2) { return !(t1 == t2); }

// Less-than operator for terms.
inline bool operator<(const Term& t1, const Term& t2) {
  return t1.index_ < t2.index_;
}

// Output operator for terms.
std::ostream& operator<<(std::ostream& os, const Term& t);

// Term table.
class TermTable {
 public:
  // Returns a fresh variable with the given type.
  static Variable add_variable(const Type& type);

  // Sets the type of the given term.
  static void set_type(const Term& term, const Type& type);

  // Returns the type of the given term.
  static const Type& type(const Term& term);

  // Constructs an empty term table.
  TermTable() : parent_(0) {}

  // Deletes this term table.
  ~TermTable();

  // Constructs a term table extending the given term table.
  TermTable(const TermTable& parent) : parent_(&parent) {}

  // Adds an object with the given name and type to this term table and returns
  // the object.
  const Object& add_object(const std::string& name, const Type& type);

  // Returns the object with the given name, or 0 if no object with the given
  // name exists.
  const Object* find_object(const std::string& name) const;

  // Returns a list with objects that are compatible with the given type.
  const std::vector<Object>& compatible_objects(const Type& type) const;

 private:
  // Object names.
  static std::vector<std::string> names_;
  // Object types.
  static std::vector<Type> object_types_;
  // Variable types.
  static std::vector<Type> variable_types_;

  // Parent term table.
  const TermTable* parent_;
  // Mapping of object names to objects.
  std::map<std::string, Object> objects_;
  // Cached results of compatible objects queries.
  mutable std::map<Type, const std::vector<Object>*> compatible_;

  friend std::ostream& operator<<(std::ostream& os, const TermTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Term& t);
};

// Output operator for term tables.
std::ostream& operator<<(std::ostream& os, const TermTable& t);

#endif  // TERMS_H_
