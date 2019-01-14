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
// PDDL functions.

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"

// A function.
class Function {
 public:
  // Constructs a function.
  explicit Function(int index) : index_(index) {}

 private:
  // Function index.
  int index_;

  friend bool operator==(const Function& f1, const Function& f2);
  friend bool operator<(const Function& f1, const Function& f2);
  friend std::ostream& operator<<(std::ostream& os, const Function& f);
  friend struct FunctionTable;
};

// Equality operator for functions.
inline bool operator==(const Function& f1, const Function& f2) {
  return f1.index_ == f2.index_;
}

// Inequality operator for functions.
inline bool operator!=(const Function& f1, const Function& f2) {
  return !(f1 == f2);
}

// Less-than operator for functions.
inline bool operator<(const Function& f1, const Function& f2) {
  return f1.index_ < f2.index_;
}

// Output operator for functions.
std::ostream& operator<<(std::ostream& os, const Function& f);

// Function table.
class FunctionTable {
 public:
  // Adds a parameter with the given type to the given function.
  static void add_parameter(const Function& function, const Type& type);

  // Returns the name of the given function.
  static const std::string& name(const Function& function);

  // Returns the parameter types of the given function.
  static const std::vector<Type>& parameters(const Function& function);

  // Makes the given function dynamic.
  static void make_dynamic(const Function& function);

  // Tests if the given function is static.
  static bool static_function(const Function& function);

  // Adds a function with the given name to this table and returns the function.
  const Function& add_function(const std::string& name);

  // Returns a pointer to the function with the given name, or 0 if no function
  // with the given name exists.
  const Function* find_function(const std::string& name) const;

 private:
  // Function names.
  static std::vector<std::string> names_;
  // Function parameters.
  static std::vector<std::vector<Type>> parameters_;
  // Static functions.
  static std::set<Function> static_functions_;

  // Mapping of function names to functions.
  std::map<std::string, Function> functions_;

  friend std::ostream& operator<<(std::ostream& os, const FunctionTable& t);
  friend std::ostream& operator<<(std::ostream& os, const Function& f);
};

// Output operator for function tables.
std::ostream& operator<<(std::ostream& os, const FunctionTable& t);

#endif  // FUNCTIONS_H_
