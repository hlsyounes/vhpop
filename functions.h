/* -*-C++-*- */
/*
 * Functions.
 *
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
 * $Id: functions.h,v 6.1 2003-12-05 23:19:15 lorens Exp $
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <config.h>
#include "types.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>


/* Function index. */
typedef int Function;

/* Name of number type. */
const std::string NUMBER_NAME("number");
/* The total-time function. */
const Function TOTAL_TIME_FUNCTION = 0;
/* Name of total-time function. */
const std::string TOTAL_TIME_NAME("total-time");


/* ====================================================================== */
/* FunctionSet */

/* Set of function declarations. */
struct FunctionSet : public std::set<Function> {
};


/* ====================================================================== */
/* FunctionTable */

/*
 * Function table.
 */
struct FunctionTable {
  /* Adds a function with the given name to this table and returns the
     function. */
  Function add_function(const std::string& name);

  /* Returns the function with the given name.  If no function with
     the given name exists, false is returned in the second part of
     the result. */
  std::pair<Function, bool> find_function(const std::string& name) const;

  /* Returns the first function of this function table. */
  Function first_function() const { return TOTAL_TIME_FUNCTION; }

  /* Returns the last function of this function table. */
  Function last_function() const { return names_.size(); }

  /* Adds a parameter with the given type to the given function. */
  void add_parameter(Function function, Type type) {
    parameters_[function - 1].push_back(type);
  }

  /* Returns the name of the given function. */
  const std::string& name(Function function) const {
    return ((function == TOTAL_TIME_FUNCTION)
	    ? TOTAL_TIME_NAME : names_[function - 1]);
  }

  /* Returns the arity of the given function. */
  size_t arity(Function function) const {
    return ((function == TOTAL_TIME_FUNCTION)
	    ? 0 : parameters_[function - 1].size());
  }

  /* Returns the ith parameter type of the given function. */
  Type parameter(Function function, size_t i) const {
    return parameters_[function - 1][i];
  }

  /* Makes the given function dynamic. */
  void make_dynamic(Function function) {
    static_functions_.erase(function);
  }

  /* Tests if the given function is static. */
  bool static_function(Function function) const {
    return static_functions_.find(function) != static_functions_.end();
  }

  /* Prints the given function on the given stream. */
  void print_function(std::ostream& os, Function function) const;

private:
  /* Function names. */
  std::vector<std::string> names_;
  /* Mapping of function names to functions. */
  std::map<std::string, Function> functions_;
  /* Function parameters. */
  std::vector<TypeList> parameters_;
  /* Static functions. */
  FunctionSet static_functions_;
};


#endif /* FUNCTIONS_H */
