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

#include "types.h"

#include <stdexcept>

std::ostream& operator<<(std::ostream& os, const Type& t) {
  if (!t.simple()) {
    const std::set<Type>& types = TypeTable::utypes_[-t.index_ - 1];
    os << "(either";
    for (std::set<Type>::const_iterator ti = types.begin(); ti != types.end();
         ti++) {
      os << ' ' << TypeTable::names_[(*ti).index_];
    }
    os << ")";
  } else if (t == TypeTable::OBJECT) {
    os << TypeTable::OBJECT_NAME;
  } else {
    os << TypeTable::names_[t.index_ - 1];
  }
  return os;
}

const Type TypeTable::OBJECT(0);
const std::string TypeTable::OBJECT_NAME("object");
const std::string TypeTable::NUMBER_NAME("number");

std::vector<std::string> TypeTable::names_;
std::vector<std::vector<bool>> TypeTable::subtype_;
std::vector<std::set<Type>> TypeTable::utypes_;

Type TypeTable::union_type(const std::set<Type>& types) {
  if (types.empty()) {
    throw std::logic_error("empty union type");
  } else if (types.size() == 1) {
    return *types.begin();
  } else {
    utypes_.push_back(types);
    return Type(-utypes_.size());
  }
}

bool TypeTable::add_supertype(const Type& type1, const Type& type2) {
  if (!type2.simple()) {
    // Add all component types of type2 as supertypes of type1.
    const std::set<Type>& types = utypes_[-type2.index_ - 1];
    for (std::set<Type>::const_iterator ti = types.begin(); ti != types.end();
         ti++) {
      if (!add_supertype(type1, *ti)) {
        return false;
      }
    }
    return true;
  } else if (subtype(type1, type2)) {
    // The first type is already a subtype of the second type.
    return true;
  } else if (subtype(type2, type1)) {
    // The second type is already a subtype of the first type.
    return false;
  } else {
    // Make all subtypes of type1 subtypes of all supertypes of type2.
    size_t n = names_.size();
    for (size_t k = 1; k <= n; k++) {
      if (subtype(Type(k), type1) && !subtype(Type(k), type2)) {
        for (size_t l = 1; l <= n; l++) {
          if (subtype(type2, Type(l))) {
            if (k > l) {
              subtype_[k - 2][2 * k - l - 2] = true;
            } else {
              subtype_[l - 2][k - 1] = true;
            }
          }
        }
      }
    }
    return true;
  }
}

bool TypeTable::subtype(const Type& type1, const Type& type2) {
  if (type1 == type2) {
    // Same types.
    return true;
  } else if (!type1.simple()) {
    // Every component type of type1 must be a subtype of type2.
    const std::set<Type>& types = utypes_[-type1.index_ - 1];
    for (std::set<Type>::const_iterator ti = types.begin(); ti != types.end();
         ti++) {
      if (!subtype(*ti, type2)) {
        return false;
      }
    }
    return true;
  } else if (!type2.simple()) {
    // type1 one must be a subtype of some component type of type2.
    const std::set<Type>& types = utypes_[-type2.index_ - 1];
    for (std::set<Type>::const_iterator ti = types.begin(); ti != types.end();
         ti++) {
      if (subtype(type1, *ti)) {
        return true;
      }
    }
    return false;
  } else if (type1 == OBJECT) {
    return false;
  } else if (type2 == OBJECT) {
    return true;
  } else if (type2 < type1) {
    return subtype_[type1.index_ - 2][2 * type1.index_ - type2.index_ - 2];
  } else {
    return subtype_[type2.index_ - 2][type1.index_ - 1];
  }
}

bool TypeTable::compatible(const Type& type1, const Type& type2) {
  return subtype(type1, type2) || subtype(type2, type1);
}

void TypeTable::components(std::set<Type>& components, const Type& type) {
  if (!type.simple()) {
    components = utypes_[-type.index_ - 1];
  } else if (type != OBJECT) {
    components.insert(type);
  }
}

const Type* TypeTable::most_specific(const Type& type1, const Type& type2) {
  if (subtype(type1, type2)) {
    return &type1;
  } else if (subtype(type2, type1)) {
    return &type2;
  } else {
    return 0;
  }
}

const Type& TypeTable::add_type(const std::string& name) {
  names_.push_back(name);
  std::pair<std::map<std::string, Type>::const_iterator, bool> ti =
      types_.insert(std::make_pair(name, names_.size()));
  const Type& type = (*ti.first).second;
  if (type.index_ > 1) {
    subtype_.push_back(std::vector<bool>(2 * (type.index_ - 1), false));
  }
  return type;
}

const Type* TypeTable::find_type(const std::string& name) const {
  std::map<std::string, Type>::const_iterator ti = types_.find(name);
  if (ti != types_.end()) {
    return &(*ti).second;
  } else {
    return 0;
  }
}

std::ostream& operator<<(std::ostream& os, const TypeTable& t) {
  for (std::map<std::string, Type>::const_iterator ti = t.types_.begin();
       ti != t.types_.end(); ti++) {
    const Type& t1 = (*ti).second;
    os << std::endl << "  " << t1;
    bool first = true;
    for (std::map<std::string, Type>::const_iterator tj = t.types_.begin();
         tj != t.types_.end(); tj++) {
      const Type& t2 = (*tj).second;
      if (t1 != t2 && TypeTable::subtype(t1, t2)) {
        if (first) {
          os << " <:";
          first = false;
        }
        os << ' ' << t2;
      }
    }
  }
  return os;
}
