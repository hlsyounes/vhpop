/* -*-C++-*- */
/*
 * Problem descriptions.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: problems.h,v 6.2 2003-07-28 21:39:25 lorens Exp $
 */
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <config.h>
#include "domains.h"
#include "formulas.h"
#include "terms.h"
#include "types.h"


/*
 * Problem definition.
 */
struct Problem {
  /* Table of problem definitions. */
  struct ProblemMap : public std::map<std::string, const Problem*> {
  };

  /* Iterator for problem tables. */
  typedef ProblemMap::const_iterator ProblemMapIter;

  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMapIter begin();

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMapIter end();

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const std::string& name);

  /* Removes all defined problems. */
  static void clear();

  /* Constructs a problem. */
  Problem(const std::string& name, const Domain& domain);

  /* Deletes a problem. */
  ~Problem();

  /* Returns the name of this problem. */
  const std::string& name() const { return name_; }

  /* Returns the domain of this problem. */
  const Domain& domain() const { return *domain_; }

  /* Returns the term table of this problem. */
  TermTable& terms() { return terms_; }

  /* Returns the term table of this problem. */
  const TermTable& terms() const { return terms_; }

  /* Adds an atomic formula to the initial conditions of this problem. */
  void add_init(const Atom& atom);

  /* Sets the goal of this problem. */
  void set_goal(const Formula& goal);

  /* Returns the initial conditions of this problem. */
  const AtomList& init() const { return init_; }

  /* Returns the action representing the initial conditions of this problem. */
  const GroundAction& init_action() const { return init_action_; }

  /* Returns the goal of this problem. */
  const Formula& goal() const { return *goal_; }

  /* Fills the provided object list with objects (including constants
     declared in the domain) that are compatible with the given
     type. */
  void compatible_objects(ObjectList& objects, Type type) const;

  /* Fills the provided action list with ground actions instantiated
     from the action schemas of the domain. */
  void instantiated_actions(GroundActionList& actions) const;

  /* Returns a new variable for this problem. */
  Variable new_variable(Type type) const;

  /* Prints the given term on the given stream with the given
     bindings. */
  void print_term(std::ostream& os, Term term, size_t step_id,
		  const Bindings& bindings) const;

private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Name of problem. */
  std::string name_;
  /* Problem domain. */
  const Domain* domain_;
  /* Problem terms. */
  mutable TermTable terms_;
  /* Initial condition of problem. */
  AtomList init_;
  /* Aciton representing initial conditions of problem. */
  GroundAction init_action_;
  /* Goal of problem. */
  const Formula* goal_;

  friend std::ostream& operator<<(std::ostream& os, const Problem& p);
};

/* Output operator for problems. */
std::ostream& operator<<(std::ostream& os, const Problem& p);


#endif /* PROBLEMS_H */
