/* -*-C++-*- */
/*
 * Problem descriptions.
 *
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: problems.h,v 3.2 2002-05-28 22:19:10 lorens Exp $
 */
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include "support.h"
#include "formulas.h"

struct Type;
struct Domain;
struct NameMap;
struct Effect;
struct Formula;
struct GroundActionList;
struct Name;
struct NameList;


/*
 * Problem definition.
 */
struct Problem : public Printable {
  /* Table of problem definitions. */
  struct ProblemMap : public hash_map<string, const Problem*> {
  };

  /* Iterator for problem tables. */
  typedef ProblemMap::const_iterator ProblemMapIter;

  /* Name of problem. */
  const string name;
  /* Problem domain. */
  const Domain& domain;

  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMapIter begin();

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMapIter end();

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const string& name);

  /* Removes all defined problems. */
  static void clear();

  /* Constructs a problem. */
  Problem(const string& name, const Domain& domain);

  /* Deletes a problem. */
  virtual ~Problem();

  /* Adds an object to this problem. */
  void add_object(const Name& object);

  /* Sets the initial conditions of this problem. */
  void set_init(const Effect& effect);

  /* Sets the goal of this problem. */
  void set_goal(const Formula& goal);

  /* Returns the object with the given name, or NULL if it is
     undefined. */
  const Name* find_object(const string& name) const;

  /* Returns the initial conditions of this problem. */
  const Effect& init() const { return *init_; }

  /* Returns the goal of this problem. */
  const Formula& goal() const { return *goal_; }

  /* Fills the provided name list with objects (including constants
     declared in the domain) that are compatible with the given
     type. */
  void compatible_objects(NameList& objects, const Type& t) const;

  /* Fills the provided action list with ground actions instantiated
     from the action schemas of the domain. */
  void instantiated_actions(GroundActionList& actions) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Problem objects. */
  NameMap objects_;
  /* Initial condition of problem. */
  const Effect* init_;
  /* Goal of problem. */
  const Formula* goal_;
};


#endif /* PROBLEMS_H */
