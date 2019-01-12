// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Ordering constraints.

#ifndef ORDERINGS_H
#define ORDERINGS_H

#include <map>
#include <vector>

#include "chain.h"
#include "formulas.h"

struct Effect;
struct Step;


/* ====================================================================== */
/* StepTime */

/*
 * A step time.
 */
struct StepTime {
  typedef enum { START, END } StepPoint;
  typedef enum { BEFORE, AT, AFTER } StepRel;

  static const StepTime AT_START;
  static const StepTime AFTER_START;
  static const StepTime BEFORE_END;
  static const StepTime AT_END;

  StepPoint point;
  StepRel rel;

private:
  StepTime(StepPoint point, StepRel rel) : point(point), rel(rel) {}
};

inline bool operator==(const StepTime& st1, const StepTime& st2) {
  return st1.point == st2.point && st1.rel == st2.rel;
}

bool operator<(const StepTime& st1, const StepTime& st2);

bool operator<=(const StepTime& st1, const StepTime& st2);

bool operator>=(const StepTime& st1, const StepTime& st2);

bool operator>(const StepTime& st1, const StepTime& st2);


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

  /* Checks if the first step could be ordered after or at the same
     time as the second step. */
  virtual bool possibly_not_before(size_t id1, StepTime t1,
				   size_t id2, StepTime t2) const = 0;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const = 0;

  /* Checks if the first step could be ordered before or at the same
     time as the second step. */
  virtual bool possibly_not_after(size_t id1, StepTime t1,
				  size_t id2, StepTime t2) const = 0;

  /* Checks if the two steps are possibly concurrent. */
  virtual bool possibly_concurrent(size_t id1, size_t id2, bool& ss, bool& se,
				   bool& es, bool& ee) const = 0;

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
  virtual float makespan(const std::map<std::pair<size_t,
			 StepTime::StepPoint>, float>& min_times) const = 0;

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

  /* Checks if the first step could be ordered after or at the same
     time as the second step. */
  virtual bool possibly_not_before(size_t id1, StepTime t1,
				   size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered before or at the same
     time as the second step. */
  virtual bool possibly_not_after(size_t id1, StepTime t1,
				  size_t id2, StepTime t2) const;

  /* Checks if the two steps are possibly concurrent. */
  virtual bool possibly_concurrent(size_t id1, size_t id2, bool& ss, bool& se,
				   bool& es, bool& ee) const;

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
  virtual float makespan(const std::map<std::pair<size_t,
			 StepTime::StepPoint>, float>& min_times) const;

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
		 const std::map<std::pair<size_t,
		 StepTime::StepPoint>, float>& min_times) const;

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

struct IntVector;

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

  /* Checks if the first step could be ordered after or at the same
     time as the second step. */
  virtual bool possibly_not_before(size_t id1, StepTime t1,
				   size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered after the second step. */
  virtual bool possibly_after(size_t id1, StepTime t1,
			      size_t id2, StepTime t2) const;

  /* Checks if the first step could be ordered before or at the same
     time as the second step. */
  virtual bool possibly_not_after(size_t id1, StepTime t1,
				  size_t id2, StepTime t2) const;

  /* Checks if the two steps are possibly concurrent. */
  virtual bool possibly_concurrent(size_t id1, size_t id2, bool& ss, bool& se,
				   bool& es, bool& ee) const;

  /* Returns the ordering collection with the given additions. */
  const TemporalOrderings* refine(size_t step_id,
				  float min_start, float min_end) const;

  /* Returns the ordering collection with the given additions. */
  const TemporalOrderings* refine(float time, const Step& new_step) const;

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
  virtual float makespan(const std::map<std::pair<size_t,
			 StepTime::StepPoint>, float>& min_times) const;

protected:
  /* Prints this opbject on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Matrix representing the minimal network for the ordering constraints. */
  std::vector<const IntVector*> distance_;
  /* Steps that are linked to the goal. */
  const Chain<size_t>* goal_achievers_;

  /* Constructs a copy of this ordering collection. */
  TemporalOrderings(const TemporalOrderings& o);

  /* Returns the time node for the given step. */
  size_t time_node(size_t id, StepTime t) const {
    return (t.point == StepTime::START) ? 2*id - 1: 2*id;
  }

  /* Returns the maximum distance from the first and the second time node. */
  int distance(size_t t1, size_t t2) const;

  /* Sets the maximum distance from the first and the second time node. */
  void set_distance(std::map<size_t, IntVector*>& own_data,
		    size_t t1, size_t t2, int d);

  /* Updates the transitive closure given a new ordering constraint. */
  bool fill_transitive(std::map<size_t, IntVector*>& own_data,
		       size_t i, size_t j, int dist);
};


#endif /* ORDERINGS_H */
