/* -*-C++-*- */
/*
 * Predicates.
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
 * $Id: predicates.h,v 6.1 2003-03-23 10:51:27 lorens Exp $
 */
#ifndef PREDICATES_H
#define PREDICATES_H

#include <config.h>
#include "types.h"
#include "hashing.h"
#include <map>
#include <string>
#include <vector>


/* Predicate index. */
typedef size_t Predicate;


/* ====================================================================== */
/* PredicateSet */

/* Set of predicate declarations. */
struct PredicateSet : public hashing::hash_set<Predicate> {
};


/* ====================================================================== */
/* PredicateTable */

/*
 * Predicate table.
 */
struct PredicateTable {
  /* Adds a predicate with the given name to this table and returns
     the predicate. */
  Predicate add_predicate(const std::string& name);

  /* Returns the predicate with the given name.  If no predicate with
     the given name exists, false is returned in the second part of
     the result. */
  std::pair<Predicate, bool> find_predicate(const std::string& name) const;

  /* Returns the size of this predicate table. */
  Predicate size() const { return names_.size(); }

  /* Returns the name of the given predicate. */
  const std::string& name(Predicate predicate) const {
    return names_[predicate];
  }

  /* Adds a parameter with the given type to the given predicate. */
  void add_parameter(Predicate predicate, Type type) {
    parameters_[predicate].push_back(type);
  }

  /* Returns the arity of the given predicate. */
  size_t arity(Predicate predicate) const {
    return parameters_[predicate].size();
  }

  /* Returns the ith parameter type of the given predicate. */
  Type parameter(Predicate predicate, size_t i) const {
    return parameters_[predicate][i];
  }

  /* Makes the given predicate dynamic. */
  void make_dynamic(Predicate predicate) {
    static_predicates_.erase(predicate);
  }

  /* Tests if the given predicate is static. */
  bool static_predicate(Predicate predicate) const {
    return static_predicates_.find(predicate) != static_predicates_.end();
  }

private:
  /* Predicate names. */
  std::vector<std::string> names_;
  /* Mapping of predicate names to predicates. */
  std::map<std::string, Predicate> predicates_;
  /* Predicate parameters. */
  std::vector<TypeList> parameters_;
  /* Static predicates. */
  PredicateSet static_predicates_;
};


#endif /* PREDICATES_H */
