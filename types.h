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
// PDDL types.

#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

// A type.
class Type {
 public:
  // Constructs a type.
  explicit Type(int index) : index_(index) {}

  // Tests if this is a simple type.
  bool simple() const { return index_ >= 0; }

 private:
  // Type index.
  int index_;

  friend bool operator==(const Type& t1, const Type& t2);
  friend bool operator<(const Type& t1, const Type& t2);
  friend std::ostream& operator<<(std::ostream& os, const Type& t);
  friend class TypeTable;
};

// Equality operator for types.
inline bool operator==(const Type& t1, const Type& t2) {
  return t1.index_ == t2.index_;
}

// Inequality operator for types.
inline bool operator!=(const Type& t1, const Type& t2) { return !(t1 == t2); }

// Less-than operator for types.
inline bool operator<(const Type& t1, const Type& t2) {
  return t1.index_ < t2.index_;
}

// Output operator for types.
std::ostream& operator<<(std::ostream& os, const Type& t);

// Type table.
class TypeTable {
 public:
  // The object type.
  static const Type OBJECT;
  // Name of object type.
  static const std::string OBJECT_NAME;
  // Name of number type.
  static const std::string NUMBER_NAME;

  // Adds a union type of the given types to this table and returns the union
  // type.
  static Type union_type(const std::set<Type>& types);

  // Adds the second type as a supertype of the first type.  Returns false if
  // the second type is a proper subtype of the first type.
  static bool add_supertype(const Type& type1, const Type& type2);

  // Tests if the first type is a subtype of the second type.
  static bool subtype(const Type& type1, const Type& type2);

  // Tests if the given types are compatible.
  static bool compatible(const Type& type1, const Type& type2);

  // Fills the provided set with the components of the given type.
  static void components(std::set<Type>& components, const Type& type);

  // Returns a pointer to the most specific of the given types, or 0 if the
  // given types are incompatible.
  static const Type* most_specific(const Type& type1, const Type& type2);

  // Adds a simple type with the given name to this table and returns the type.
  const Type& add_type(const std::string& name);

  // Returns a pointer to the type with the given name, or 0 if no type with the
  // given name exists in this table.
  const Type* find_type(const std::string& name) const;

 private:
  // Type names.
  static std::vector<std::string> names_;
  // Transitive closure of subtype relation.
  static std::vector<std::vector<bool>> subtype_;
  // Union types.
  static std::vector<std::set<Type>> utypes_;

  // Mapping of type names to types.
  std::map<std::string, Type> types_;

  friend std::ostream& operator<<(std::ostream& os, const TypeTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Type& t);
};

// Output operator for type tables.
std::ostream& operator<<(std::ostream& os, const TypeTable& t);

#endif  // TYPES_H_
