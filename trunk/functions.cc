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
 * $Id: functions.cc,v 6.1 2003-12-05 23:19:19 lorens Exp $
 */
#include "functions.h"


/* ====================================================================== */
/* FunctionTable */

/* Adds a function with the given name to this table and returns the
   function. */
Function FunctionTable::add_function(const std::string& name) {
  Function function = last_function() + 1;
  names_.push_back(name);
  functions_.insert(std::make_pair(name, function));
  parameters_.push_back(TypeList());
  static_functions_.insert(function);
  return function;
}


/* Returns the function with the given name.  If no function with
   the given name exists, false is returned in the second part of
   the result. */
std::pair<Function, bool>
FunctionTable::find_function(const std::string& name) const {
  std::map<std::string, Function>::const_iterator fi = functions_.find(name);
  if (fi != functions_.end()) {
    return std::make_pair((*fi).second, true);
  } else {
    return std::make_pair(0, false);
  }
}


/* Prints the given function on the given stream. */
void FunctionTable::print_function(std::ostream& os, Function function) const {
  os << name(function);
}
