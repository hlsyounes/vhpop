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

#include "functions.h"

std::ostream& operator<<(std::ostream& os, const Function& f) {
  os << FunctionTable::names_[f.index_];
  return os;
}

std::vector<std::string> FunctionTable::names_;
std::vector<std::vector<Type>> FunctionTable::parameters_;
std::set<Function> FunctionTable::static_functions_;

void FunctionTable::add_parameter(const Function& function, const Type& type) {
  parameters_[function.index_].push_back(type);
}

const std::string& FunctionTable::name(const Function& function) {
  return names_[function.index_];
}

const std::vector<Type>& FunctionTable::parameters(const Function& function) {
  return parameters_[function.index_];
}

void FunctionTable::make_dynamic(const Function& function) {
  static_functions_.erase(function);
}

bool FunctionTable::static_function(const Function& function) {
  return static_functions_.find(function) != static_functions_.end();
}

const Function& FunctionTable::add_function(const std::string& name) {
  std::pair<std::map<std::string, Function>::const_iterator, bool> fi =
      functions_.insert(std::make_pair(name, Function(names_.size())));
  const Function& function = (*fi.first).second;
  names_.push_back(name);
  parameters_.push_back(std::vector<Type>());
  static_functions_.insert(function);
  return function;
}

const Function* FunctionTable::find_function(const std::string& name) const {
  std::map<std::string, Function>::const_iterator fi = functions_.find(name);
  if (fi != functions_.end()) {
    return &(*fi).second;
  } else {
    return 0;
  }
}

std::ostream& operator<<(std::ostream& os, const FunctionTable& t) {
  for (std::map<std::string, Function>::const_iterator fi =
           t.functions_.begin();
       fi != t.functions_.end(); fi++) {
    const Function& f = (*fi).second;
    os << std::endl << "  (" << f;
    const std::vector<Type>& types = FunctionTable::parameters(f);
    for (std::vector<Type>::const_iterator ti = types.begin();
         ti != types.end(); ti++) {
      os << " ?v - " << *ti;
    }
    os << ") - " << TypeTable::NUMBER_NAME;
    if (FunctionTable::static_function(f)) {
      os << " <static>";
    }
  }
  return os;
}
