/* -*-C++-*- */
/*
 * Ordering constraints.
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
 * $Id: orderings.h,v 3.1 2002-03-10 14:34:46 lorens Exp $
 */
#ifndef ORDERINGS_H
#define ORDERINGS_H

#include "support.h"
#include "chain.h"

struct Reason;
struct Step;
struct Literal;
struct Effect;


/*
 * A step time.
 */
typedef enum { STEP_START, STEP_END } StepTime;

/* Returns the step time corresponding to the end time of the given
   effect. */
StepTime end_time(const Effect& e);

/* Returns the step time corresponding to the end time of the given
   literal. */
StepTime end_time(const Literal& f);

/* Returns the step time corresponding to the start time of the given
   literal. */
StepTime start_time(const Literal& f);


/*
 * Ordering constraint between plan steps.
 */
struct Ordering : public Printable, public gc {
  /* Preceeding step. */
  const size_t before_id;
  /* Time point of preceeding step. */
  StepTime t1;
  /* Succeeding step. */
  const size_t after_id;
  /* Time point of suceeding step. */
  StepTime t2;
  /* Reason for ordering constraint. */
  const Reason& reason;

  /* Constructs an ordering constraint. */
  Ordering(size_t before_id, StepTime t1, size_t after_id, StepTime t2,
	   const Reason& reason);

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
  /* Returns the ordering constraints making up this collection. */
  const OrderingChain* orderings() const;

  /* Checks if the first step could be ordered before the second step. */
  virtual bool possibly_before(size_t id1, StepTime t1,
			       size_t id2, StepTime t2) const = 0;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const = 0;

  /* Returns the ordering collection with the given addition. */
  virtual const Orderings* refine(const Ordering& new_ordering) const = 0;

  /* Returns the ordering collection with the given additions. */
  virtual const Orderings* refine(const Ordering& new_ordering,
				  const Step& new_step) const = 0;

  /* Fills the given table with distances for each step to the goal
     step, and returns the greatest distance. */
  virtual float goal_distances(hash_map<size_t, float>& start_dist,
			       hash_map<size_t, float>& end_dist) const;

protected:
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

  /* Constructs an empty ordering collection. */
  Orderings();

  /* Returns the distance of the given step to the goal step, and also
     enters it into the given distance table. */
  virtual float goal_distance(hash_map<size_t, float>& start_dist,
			      hash_map<size_t, float>& end_dist,
			      size_t step_id,
			      StepTime t = STEP_START) const = 0;

  /* Updates the transitive closure given new ordering constraints. */
  bool fill_transitive(const OrderingChain* orderings);

  /* Updates the transitive closure given a new ordering constraint. */
  virtual bool fill_transitive(const Ordering& ordering) = 0;

  /* Prints this ordering collection on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Collection of binary ordering constraints.
 */
struct BinaryOrderings : public Orderings {
  /* Constructs an empty ordering collection. */
  BinaryOrderings();

  /* Constructs an ordering collection. */
  BinaryOrderings(const Chain<const Step*>* steps,
		  const OrderingChain* orderings);

  /* Computes the flexibility of this ordering collection as defined in
     "Reviving Partial Order Planning" (Nguyen & Kambhampati 2001). */
  float flexibility() const;

  /* Checks if the first step could be ordered before the second step. */
  virtual bool possibly_before(size_t id1, StepTime t1,
			       size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Returns the ordering collection with the given addition. */
  virtual const Orderings* refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  virtual const Orderings* refine(const Ordering& new_ordering,
				  const Step& new_step) const;

protected:
  /* Returns the distance of the given step to the goal step, and also
     enters it into the given distance table. */
  virtual float goal_distance(hash_map<size_t, float>& start_dist,
			      hash_map<size_t, float>& end_dist,
			      size_t step_id, StepTime t = STEP_START) const;

  /* Updates the transitive closure given a new ordering constraint. */
  virtual bool fill_transitive(const Ordering& ordering);
};


/*
 * Collection of temporal ordering constraints.
 */
struct TemporalOrderings : public Orderings {
  /* Constructs an empty ordering collection. */
  TemporalOrderings();

  /* Constructs an ordering collection. */
  TemporalOrderings(const Chain<const Step*>* steps,
		    const OrderingChain* orderings);

  /* Checks if the first step could be ordered before the second step. */
  virtual bool possibly_before(size_t id1, StepTime t1,
			       size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Returns the ordering collection with the given addition. */
  virtual const Orderings* refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  virtual const Orderings* refine(const Ordering& new_ordering,
				  const Step& new_step) const;

protected:
  /* Returns the distance of the given step to the goal step, and also
     enters it into the given distance table. */
  virtual float goal_distance(hash_map<size_t, float>& start_dist,
			      hash_map<size_t, float>& end_dist,
			      size_t step_id, StepTime t = STEP_START) const;

  /* Updates the transitive closure given a new ordering constraint. */
  virtual bool fill_transitive(const Ordering& ordering);

private:
  /* Matrix representing the minimal network for the ordering constraints. */
  vector<vector<float> > distance_;

  /* Returns the time node for the given step. */
  size_t time_node(size_t id, StepTime t) const;
};


#endif /* ORDERINGS_H */
