/* -*-C++-*- */
/*
 * Problem descriptions.
 *
 * $Id: problems.h,v 1.7 2001-10-08 03:09:06 lorens Exp $
 */
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include "support.h"
#include "types.h"


struct Domain;
struct NameMap;
struct Effect;
struct Formula;
struct ActionList;
struct Name;
struct NameList;


/*
 * Problem definition.
 */
struct Problem : public Printable, public gc {
  struct ProblemMap : public HashMap<string, const Problem*> {
  };

  /* Name of problem. */
  const string name;
  /* Problem domain. */
  const Domain& domain;
  /* Problem objects. */
  const NameMap& objects;
  /* Initial condition of problem. */
  const Effect& init;
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
	  const Effect& init, const Formula& goal)
    : name(name), domain(domain), objects(objects), init(init), goal(goal) {
    problems[name] = this;
  }

  /* Deletes a problem. */
  virtual ~Problem() {
    problems.erase(name);
  }

  /* Returns the object with the given name, or NULL if it is
     undefined. */
  const Name* find_object(const string& name) const;

  /* Fills the provided name list with objects (including constants
     declared in the domain) that are compatible with the given
     type. */
  void compatible_objects(NameList& objects,
			  const Type& t = SimpleType::OBJECT) const;

  /* Fills the provided action list with ground actions instantiated
     from the action schemas of the domain. */
  void instantiated_actions(ActionList& actions) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Table of defined problems. */
  static ProblemMap problems;
};

#endif /* PROBLEMS_H */
