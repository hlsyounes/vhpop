/* -*-C++-*- */
/*
 * Ordering constraints.
 *
 * $Id: orderings.h,v 1.3 2002-01-19 22:26:20 lorens Exp $
 */
#ifndef ORDERINGS_H
#define ORDERINGS_H

#include "support.h"
#include "chain.h"

struct Reason;
struct Step;


/*
 * Ordering constraint between plan steps.
 */
struct Ordering : public Printable, public gc {
  /* Preceeding step. */
  const size_t before_id;
  /* Succeeding step. */
  const size_t after_id;
  /* Reason for ordering constraint. */
  const Reason& reason;

  /* Constructs an ordering constraint. */
  Ordering(size_t before_id, size_t after_id, const Reason& reason);

protected:
  /* Prints this ordering constraint on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of ordering constraints.
 */
typedef Chain<const Ordering*> OrderingChain;


/*
 * Collection of ordering constraints.
 */
struct Orderings : public Printable, public gc {
  /* Constructs an empty ordering collection. */
  Orderings();

  /* Constructs an ordering collection. */
  Orderings(const Chain<const Step*>* steps, const OrderingChain* orderings);

  /* Returns the ordering constraints making up this collection. */
  const OrderingChain* orderings() const {
    return orderings_;
  }

  /* Checks if the first step is ordered before the second step. */
  bool before(size_t id1, size_t id2) const;

  /* Checks if the first step is ordered after the second step. */
  bool after(size_t id1, size_t id2) const;

  /* Checks if the first step could be ordered before the second step. */
  bool possibly_before(size_t id1, size_t id2) const;

  /* Checks if the first step could be ordered after the second step. */
  bool possibly_after(size_t id1, size_t id2) const;

  /* Computes the flexibility of this ordering collection as defined in
     "Reviving Partial Order Planning" (Nguyen & Kambhampati 2001). */
  double flexibility() const;

  /* Returns the ordering collection with the given addition. */
  const Orderings& refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  const Orderings& refine(const Ordering& new_ordering, size_t new_step) const;

  /* Returns the distance of the given step to the goal step, and also
     enters it into the given distance table. */
  size_t goal_distance(hash_map<size_t, size_t>& dist, size_t step_id) const;

  /* Fills the given table with distances for each step to the goal
     step, and returns the greatest distance. */
  size_t goal_distances(hash_map<size_t, size_t>& dist) const;

protected:
  /* Prints this ordering collection on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* A step id map. */
  struct IdMap : public hash_map<size_t, size_t> {
  };

  /* Iterator for id maps. */
  typedef IdMap::const_iterator IdMapIter;

  /* The ordering constraints making up this collection. */
  const OrderingChain* orderings_;
  /* Number of steps. */
  size_t size_;
  /* Maps step ids to positions in the matrix below. */
  IdMap id_map1_;
  /* Maps positions in the matrix below to step ids */
  vector<size_t> id_map2_;
  /* Matrix representing the transitive closure of the ordering constraints. */
  vector<vector<bool> > order_;

  /* Updates the transitive closure given new ordering constraints. */
  void fill_transitive(const OrderingChain* orderings);

  /* Updates the transitive closure given a new ordering constraint. */
  void fill_transitive(const Ordering& ordering);
};


#endif /* ORDERINGS_H */
