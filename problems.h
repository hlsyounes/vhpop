/* -*-C++-*- */
/*
 * Problem descriptions.
 *
 * $Id: problems.h,v 1.1 2001-05-03 15:27:07 lorens Exp $
 */
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include <iostream>
#include <string>
#include <hash_map>
#include "support.h"


struct Formula;
struct Effect;
struct Domain;


/*
 * Problem definition.
 */
struct Problem : public gc {
  typedef hash_map<string, const Problem*, hash<string>, equal_to<string>,
    container_alloc> ProblemMap;

  /* Name of problem. */
  const string name;
  /* Problem domain. */
  const Domain& domain;
  /* Initial condition of problem, or NULL if problem lacks initial
     condition. */
  const Effect* const init;
  /* Goal of problem. */
  const Formula& goal;

  /* Returns a const_iterator pointing to the first problem. */
  static ProblemMap::const_iterator begin() {
    return problems.begin();
  }

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMap::const_iterator end() {
    return problems.end();
  }

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const string& name) {
    ProblemMap::const_iterator i = problems.find(name);
    return (i != problems.end()) ? (*i).second : NULL;
  }

  /* Removes all defined problems. */
  static void clear() {
    problems.clear();
  }

  /* Constructs a problem. */
  Problem(const string& name, const Domain& domain, const NameMap& objects,
	  const Effect* init, const Formula& goal)
    : name(name), domain(domain), init(init), goal(goal), objects_(objects) {
    problems[name] = this;
  }

  /* Deletes a problem. */
  ~Problem() {
    problems.erase(name);
  }

  /* Returns the object with the given name, or NULL if it is undefined. */
  const Name* find_object(const string& name) const {
    NameMap::const_iterator i = objects_.find(name);
    return (i != objects_.end()) ? (*i).second : NULL;
  }

private:
  /* Table of defined problems. */
  static ProblemMap problems;

  /* Problem objects. */
  const NameMap& objects_;

  /* Prints this problem on the given stream. */
  void print(ostream& os) const;

  friend ostream& operator<<(ostream& os, const Problem& p);
};

/* Output operator for problems. */
inline ostream& operator<<(ostream& os, const Problem& p) {
  p.print(os);
  return os;
}

#endif /* PROBLEMS_H */
