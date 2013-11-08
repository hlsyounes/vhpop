/* -*-C++-*- */
/*
 * Ordering constraints.
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
 * $Id: orderings.h,v 3.19 2003-03-01 18:51:02 lorens Exp $
 */
#ifndef ORDERINGS_H
#define ORDERINGS_H

#include <config.h>
#include "chain.h"
#include "hashing.h"

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

  /* Constructs an ordering constraint. */
  Ordering(const Ordering& o);

  /* Deletes this ordering constraint. */
  ~Ordering();

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

/*
 * Collection of ordering constraints.
 */
struct Orderings {
  /* Register use of this object. */
  static void register_use(const Orderings* o) {
    if (o != NULL) {
      o->ref_count_++;
    }
  }

  /* Unregister use of this object. */
  static void unregister_use(const Orderings* o) {
    if (o != NULL) {
      o->ref_count_--;
      if (o->ref_count_ == 0) {
	delete o;
      }
    }
  }

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

  /* Fills the given tables with distances for each step from the
     start step, and returns the greatest distance. */
  float schedule(hashing::hash_map<size_t, float>& start_times,
		 hashing::hash_map<size_t, float>& end_times) const;

protected:
  /* A step id map. */
  struct IdMap : public hashing::hash_map<size_t, size_t> {
  };

  /* Iterator for id maps. */
  typedef IdMap::const_iterator IdMapIter;

  /* Maps step ids to positions in the matrix below. */
  IdMap id_map1_;
  /* Maps positions in the matrix below to step ids */
  std::vector<size_t> id_map2_;
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

  /* Schedules the given instruction. */
  virtual float schedule(hashing::hash_map<size_t, float>& start_times,
			 hashing::hash_map<size_t, float>& end_times,
			 size_t step_id) const = 0;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

private:
  /* Reference counter. */
  mutable size_t ref_count_;

  friend std::ostream& operator<<(std::ostream& os, const Orderings& o);
};

/* Output operator for orderings. */
std::ostream& operator<<(std::ostream& os, const Orderings& o);


/* ====================================================================== */
/* BinaryOrderings */

struct BoolVector;

/*
 * Collection of binary ordering constraints.
 */
struct BinaryOrderings : public Orderings {
  /* Constructs an empty ordering collection. */
  BinaryOrderings();

  /* Constructs an ordering collection. */
  BinaryOrderings(const CollectibleChain<Step>* steps,
		  const OrderingChain* orderings);

  /* Deletes this ordering collection. */
  virtual ~BinaryOrderings();

  /* Checks if the first step could be ordered before the second step. */
  virtual bool possibly_before(size_t id1, StepTime t1,
			       size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Checks if the two steps are possibly concurrent. */
  bool possibly_concurrent(size_t id1, StepTime t1,
			   size_t id2, StepTime t2) const;

  /* Returns the ordering collection with the given addition. */
  virtual const BinaryOrderings* refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  virtual const BinaryOrderings* refine(const Ordering& new_ordering,
					 const Step& new_step) const;

protected:
  /* Schedules the given instruction. */
  virtual float schedule(hashing::hash_map<size_t, float>& start_times,
			 hashing::hash_map<size_t, float>& end_times,
			 size_t step_id) const;

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Difference from parent's matrix representing the transitive
     closure of the ordering constraints. */
  std::vector<const BoolVector*> order_;

  /* Constructs a copy of this ordering collection. */
  BinaryOrderings(const BinaryOrderings& o);

  /* Returns the entry at (r,c) in the matrix representing the
     transitive closure of the ordering constraints. */
  bool order(size_t r, size_t c) const;

  /* Sets the entry at (r,c) to true in the matrix representing the
     transitive closure of the ordering constraints. */
  void set_order(hashing::hash_map<size_t, BoolVector*>& own_data,
		 size_t r, size_t c);

  /* Updates the transitive closure given a new ordering constraint. */
  void fill_transitive(hashing::hash_map<size_t, BoolVector*>& own_data,
		       const Ordering& ordering);
};


/* ====================================================================== */
/* TemporalOrderings */

struct FloatVector;

/*
 * Collection of temporal ordering constraints.
 */
struct TemporalOrderings : public Orderings {
  /* Minimum distance between two ordered steps. */
  static float threshold;

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
  virtual const TemporalOrderings* refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  virtual const TemporalOrderings* refine(const Ordering& new_ordering,
					  const Step& new_step) const;

protected:
  /* Schedules the given instruction. */
  virtual float schedule(hashing::hash_map<size_t, float>& start_times,
			 hashing::hash_map<size_t, float>& end_times,
			 size_t step_id) const;

  /* Prints this opbject on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Matrix representing the minimal network for the ordering constraints. */
  std::vector<const FloatVector*> distance_;

  /* Constructs a copy of this ordering collection. */
  TemporalOrderings(const TemporalOrderings& o);

  /* Returns the entry at (r,c) in the matrix representing the minimal
     network for the ordering constraints. */
  float distance(size_t r, size_t c) const;

  /* Sets the entry at (r,c) in the matrix representing the minimal
     network for the ordering constraints. */
  void set_distance(hashing::hash_map<size_t, FloatVector*>& own_data,
		    size_t r, size_t c, float d);

  /* Returns the time node for the given step. */
  size_t time_node(size_t id, StepTime t) const;

  /* Updates the transitive closure given a new ordering constraint. */
  bool fill_transitive(hashing::hash_map<size_t, FloatVector*>& own_data,
		       const Ordering& ordering);
};


#endif /* ORDERINGS_H */
