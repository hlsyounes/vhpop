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
 * $Id: orderings.h,v 3.6 2002-03-28 23:34:31 lorens Exp $
 */
#ifndef ORDERINGS_H
#define ORDERINGS_H

#include "support.h"
#include "chain.h"

struct Reason;
struct Step;
struct Literal;
struct Effect;


/* ====================================================================== */
/* StepTime */

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


/* ====================================================================== */
/* Ordering */

/*
 * Ordering constraint between plan steps.
 */
struct Ordering {
  /* Constructs an ordering constraint. */
  Ordering(size_t before_id, StepTime before_time,
	   size_t after_id, StepTime after_time, const Reason& reason);

  /* Returns the preceeding step id. */
  size_t before_id() const { return before_id_; }

  /* Returns the time point of preceeding step. */
  StepTime before_time() const { return before_time_; }

  /* Returns the suceeding step id. */
  size_t after_id() const { return after_id_; }

  /* Returns the time point of the preceeding step. */
  StepTime after_time() const { return after_time_; }

  /* Returns the reason. */
  const Reason& reason() const;

private:
  /* Preceeding step id. */
  size_t before_id_;
  /* Time point of preceeding step. */
  StepTime before_time_;
  /* Succeeding step id. */
  size_t after_id_;
  /* Time point of suceeding step. */
  StepTime after_time_;
#ifdef TRANSFORMATIONAL
  /* Reason for ordering constraint. */
  const Reason* reason_;
#endif
};


/* ====================================================================== */
/* OrderingChain */

/*
 * Chain of ordering constraints.
 */
typedef CollectibleChain<Ordering> OrderingChain;


/* ====================================================================== */
/* Orderings */

struct BoolVector;

/*
 * Collection of ordering constraints.
 */
struct Orderings : public Printable, public Collectible {
  /* Deletes this ordering collection. */
  virtual ~Orderings();

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

  /* Maps step ids to positions in the matrix below. */
  IdMap id_map1_;
  /* Maps positions in the matrix below to step ids */
  vector<size_t> id_map2_;
  /* Difference from parent's matrix representing the transitive
     closure of the ordering constraints. */
  vector<const BoolVector*> order_;
#ifdef TRANSFORMATIONAL
  /* The ordering constraints making up this collection. */
  const OrderingChain* orderings_;
#endif

  /* Constructs an empty ordering collection. */
  Orderings();

  /* Constructs a copy of this ordering collection. */
  Orderings(const Orderings& o);

  /* Returns the number of steps. */
  size_t size() const;

  /* Returns the entry at (r,c) in the matrix representing the
     transitive closure of the ordering constraints. */
  bool order(size_t r, size_t c) const;

  /* Sets the entry at (r,c) to true in the matrix representing the
     transitive closure of the ordering constraints. */
  void set_order(hash_map<size_t, BoolVector*>& own_data, size_t r, size_t c);

  /* Returns the distance of the given step to the goal step, and also
     enters it into the given distance table. */
  virtual float goal_distance(hash_map<size_t, float>& start_dist,
			      hash_map<size_t, float>& end_dist,
			      size_t step_id,
			      StepTime t = STEP_START) const = 0;

  /* Prints this ordering collection on the given stream. */
  virtual void print(ostream& os) const;
};


/* ====================================================================== */
/* BinaryOrderings */

/*
 * Collection of binary ordering constraints.
 */
struct BinaryOrderings : public Orderings {
  /* Constructs an empty ordering collection. */
  BinaryOrderings();

  /* Constructs an ordering collection. */
  BinaryOrderings(const CollectibleChain<Step>* steps,
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

private:
  /* Constructs a copy of this ordering collection. */
  BinaryOrderings(const BinaryOrderings& o);

  /* Checks if the first step is ordered before the second step. */
  bool before(size_t id1, size_t id2) const;

  /* Updates the transitive closure given a new ordering constraint. */
  void fill_transitive(hash_map<size_t, BoolVector*>& own_data,
		       const Ordering& ordering);
};


/* ====================================================================== */
/* TemporalOrderings */

struct FloatVector;

/*
 * Collection of temporal ordering constraints.
 */
struct TemporalOrderings : public Orderings {
  /* Constructs an empty ordering collection. */
  TemporalOrderings();

  /* Constructs an ordering collection. */
  TemporalOrderings(const CollectibleChain<Step>* steps,
		    const OrderingChain* orderings);

  /* Deletes this ordering collection. */
  virtual ~TemporalOrderings();

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

private:
  /* Matrix representing the minimal network for the ordering constraints. */
  vector<const FloatVector*> distance_;

  /* Constructs a copy of this ordering collection. */
  TemporalOrderings(const TemporalOrderings& o);

  /* Returns the entry at (r,c) in the matrix representing the minimal
     network for the ordering constraints. */
  float distance(size_t r, size_t c) const;

  /* Sets the entry at (r,c) in the matrix representing the minimal
     network for the ordering constraints. */
  void set_distance(hash_map<size_t, FloatVector*>& own_data,
		    size_t r, size_t c, float d);

  /* Returns the time node for the given step. */
  size_t time_node(size_t id, StepTime t) const;

  /* Checks if the first step is ordered before the second step. */
  bool before(size_t id1, StepTime t1, size_t id2, StepTime t2) const;

  /* Updates the transitive closure given a new ordering constraint. */
  bool fill_transitive(hash_map<size_t, BoolVector*>& own_data1,
		       hash_map<size_t, FloatVector*>& own_data2,
		       const Ordering& ordering);
};


#endif /* ORDERINGS_H */
