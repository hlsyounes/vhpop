/* -*-C++-*- */
/*
 * Problem descriptions.
 *
 * $Id: problems.h,v 1.10 2001-12-27 20:00:07 lorens Exp $
 */
#ifndef PROBLEMS_H
#define PROBLEMS_H

#include "support.h"

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
struct Problem : public Printable, public gc {
  /* Table of problem definitions. */
  struct ProblemMap : public HashMap<string, const Problem*> {
  };

  /* Iterator for problem tables. */
  typedef ProblemMap::const_iterator ProblemMapIter;

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
  static ProblemMapIter begin();

  /* Returns a const_iterator pointing beyond the last problem. */
  static ProblemMapIter end();

  /* Returns the problem with the given name, or NULL if it is undefined. */
  static const Problem* find(const string& name);

  /* Removes all defined problems. */
  static void clear();

  /* Constructs a problem. */
  Problem(const string& name, const Domain& domain, const NameMap& objects,
	  const Effect& init, const Formula& goal);

  /* Deletes a problem. */
  virtual ~Problem();

  /* Returns the object with the given name, or NULL if it is
     undefined. */
  const Name* find_object(const string& name) const;

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
};


#endif /* PROBLEMS_H */
