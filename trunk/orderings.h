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
 * $Id: orderings.h,v 6.4 2003-09-08 21:28:07 lorens Exp $
 */
#ifndef ORDERINGS_H
#define ORDERINGS_H

#include <config.h>
#include "formulas.h"

struct Step;
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
   formula time. */
StepTime end_time(FormulaTime ft);

/* Returns the step time corresponding to the start time of the given
   formula time. */
StepTime start_time(FormulaTime ft);


/* ====================================================================== */
/* Ordering */

/*
 * Ordering constraint between plan steps.
 */
struct Ordering {
  /* Constructs an ordering constraint. */
  Ordering(size_t before_id, StepTime before_time,
	   size_t after_id, StepTime after_time)
    : before_id_(before_id), before_time_(before_time),
      after_id_(after_id), after_time_(after_time) {}

  /* Returns the preceeding step id. */
  size_t before_id() const { return before_id_; }

  /* Returns the time point of preceeding step. */
  StepTime before_time() const { return before_time_; }

  /* Returns the suceeding step id. */
  size_t after_id() const { return after_id_; }

  /* Returns the time point of the preceeding step. */
  StepTime after_time() const { return after_time_; }

private:
  /* Preceeding step id. */
  size_t before_id_;
  /* Time point of preceeding step. */
  StepTime before_time_;
  /* Succeeding step id. */
  size_t after_id_;
  /* Time point of suceeding step. */
  StepTime after_time_;
};


/* ====================================================================== */
/* Orderings */

/*
 * Collection of ordering constraints.
 */
struct Orderings {
  /* Minimum distance between two ordered steps. */
  static float threshold;

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
				  const Step& new_step,
				  const PlanningGraph* pg,
				  const Bindings* bindings) const = 0;

  /* Fills the given tables with distances for each step from the
     start step, and returns the greatest distance. */
  virtual float schedule(std::map<size_t, float>& start_times,
			 std::map<size_t, float>& end_times) const = 0;

  /* Returns the makespan of this ordering collection. */
  virtual float makespan(const std::map<std::pair<size_t, StepTime>,
			 float>& min_times) const = 0;

protected:
  /* Constructs an empty ordering collection. */
  Orderings();

  /* Constructs a copy of this ordering collection. */
  Orderings(const Orderings& o);

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
					const Step& new_step,
					const PlanningGraph* pg,
					const Bindings* bindings) const;

  /* Fills the given tables with distances for each step from the
     start step, and returns the greatest distance. */
  virtual float schedule(std::map<size_t, float>& start_times,
			 std::map<size_t, float>& end_times) const;

  /* Returns the makespan of this ordering collection. */
  virtual float makespan(const std::map<std::pair<size_t, StepTime>,
			 float>& min_times) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Matrix representing the transitive closure of the ordering
     constraints. */
  std::vector<const BoolVector*> before_;

  /* Constructs a copy of this ordering collection. */
  BinaryOrderings(const BinaryOrderings& o);

  /* Schedules the given instruction with the given constraints. */
  float schedule(std::map<size_t, float>& start_times,
		 std::map<size_t, float>& end_times, size_t step_id) const;

  /* Schedules the given instruction with the given constraints. */
  float schedule(std::map<size_t, float>& start_times,
		 std::map<size_t, float>& end_times, size_t step_id,
		 const std::map<std::pair<size_t, StepTime>,
		 float>& min_times) const;

  /* Returns true iff the first step is ordered before the second step. */
  bool before(size_t id1, size_t id2) const;

  /* Orders the first step before the second step. */
  void set_before(std::map<size_t, BoolVector*>& own_data,
		  size_t id1, size_t id2);

  /* Updates the transitive closure given a new ordering constraint. */
  void fill_transitive(std::map<size_t, BoolVector*>& own_data,
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

  /* Deletes this ordering collection. */
  virtual ~TemporalOrderings();

  /* Checks if the first step could be ordered before the second step. */
  virtual bool possibly_before(size_t id1, StepTime t1,
			       size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Returns the the ordering collection with the given additions. */
  const TemporalOrderings* refine(size_t step_id,
				  float min_start, float min_end) const;

  /* Returns the ordering collection with the given addition. */
  virtual const TemporalOrderings* refine(const Ordering& new_ordering) const;

  /* Returns the ordering collection with the given additions. */
  virtual const TemporalOrderings* refine(const Ordering& new_ordering,
					  const Step& new_step,
					  const PlanningGraph* pg,
					  const Bindings* bindings) const;

  /* Fills the given tables with distances for each step from the
     start step, and returns the greatest distance. */
  virtual float schedule(std::map<size_t, float>& start_times,
			 std::map<size_t, float>& end_times) const;

  /* Returns the makespan of this ordering collection. */
  virtual float makespan(const std::map<std::pair<size_t, StepTime>,
			 float>& min_times) const;

protected:
  /* Prints this opbject on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Matrix representing the minimal network for the ordering constraints. */
  std::vector<const FloatVector*> distance_;

  /* Constructs a copy of this ordering collection. */
  TemporalOrderings(const TemporalOrderings& o);

  /* Returns the time node for the given step. */
  size_t time_node(size_t id, StepTime t) const {
    return (t == STEP_START) ? 2*id - 1: 2*id;
  }

  /* Returns the maximum distance from the first and the second time node. */
  float distance(size_t t1, size_t t2) const;

  /* Sets the maximum distance from the first and the second time node. */
  void set_distance(std::map<size_t, FloatVector*>& own_data,
		    size_t t1, size_t t2, float d);

  /* Updates the transitive closure given a new ordering constraint. */
  bool fill_transitive(std::map<size_t, FloatVector*>& own_data,
		       size_t i, size_t j, float dist);
};


#endif /* ORDERINGS_H */
