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

#include "orderings.h"

#include <limits.h>
#include <limits>

#include "debug.h"
#include "domains.h"
#include "expressions.h"
#include "heuristics.h"
#include "plans.h"
#include "refcount.h"

/* ====================================================================== */
/* StepTime */

const StepTime StepTime::AT_START(StepTime::START, StepTime::AT);
const StepTime StepTime::AFTER_START(StepTime::START, StepTime::AFTER);
const StepTime StepTime::BEFORE_END(StepTime::END, StepTime::BEFORE);
const StepTime StepTime::AT_END(StepTime::END, StepTime::AT);


bool operator<(const StepTime& st1, const StepTime& st2) {
  return (st1.point < st2.point
	  || (st1.point == st2.point && st1.rel < st2.rel));
}


bool operator<=(const StepTime& st1, const StepTime& st2) {
  return (st1.point <= st2.point
	  || (st1.point == st2.point && st1.rel <= st2.rel));
}


bool operator>=(const StepTime& st1, const StepTime& st2) {
  return (st1.point >= st2.point
	  || (st1.point == st2.point && st1.rel >= st2.rel));
}


bool operator>(const StepTime& st1, const StepTime& st2) {
  return (st1.point > st2.point
	  || (st1.point == st2.point && st1.rel > st2.rel));
}


/* Returns the step time corresponding to the end time of the given
   effect. */
StepTime end_time(const Effect& e) {
  if (e.when() == Effect::AT_START) {
    return StepTime::AT_START;
  } else {
    return StepTime::AT_END;
  }
}


/* Returns the step time corresponding to the end time of the given
   formula time. */
StepTime end_time(FormulaTime ft) {
  if (ft == AT_START) {
    return StepTime::AT_START;
  } else if (ft == AT_END) {
    return StepTime::AT_END;
  } else {
    return StepTime::BEFORE_END;
  }
}


/* Returns the step time corresponding to the start time of the given
   literal. */
StepTime start_time(FormulaTime ft) {
  if (ft == AT_START) {
    return StepTime::AT_START;
  } else if (ft == AT_END) {
    return StepTime::AT_END;
  } else {
    return StepTime::AFTER_START;
  }
}


/* ====================================================================== */
/* BoolVector */

/*
 * A collectible bool vector.
 */
struct BoolVector : public std::vector<bool> {
  /* Register use of the given vector. */
  static void register_use(const BoolVector* v) {
    if (v != NULL) {
      v->ref_count_++;
    }
  }

  /* Unregister use of the given vector. */
  static void unregister_use(const BoolVector* v) {
    if (v != NULL) {
      v->ref_count_--;
      if (v->ref_count_ == 0) {
	delete v;
      }
    }
  }

  /* Constructs a vector with n copies of b. */
  BoolVector(size_t n, bool b)
    : std::vector<bool>(n, b), ref_count_(0) {
  }

  /* Constructs a copy of the given vector. */
  BoolVector(const BoolVector& v)
    : std::vector<bool>(v), ref_count_(0) {
  }

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* IntVector */

/*
 * A collectible int vector.
 */
struct IntVector : public std::vector<int> {
  /* Register use of the given vector. */
  static void register_use(const IntVector* v) {
    if (v != NULL) {
      v->ref_count_++;
    }
  }

  /* Unregister use of the given vector. */
  static void unregister_use(const IntVector* v) {
    if (v != NULL) {
      v->ref_count_--;
      if (v->ref_count_ == 0) {
	delete v;
      }
    }
  }

  /* Constructs a vector with n copies of b. */
  IntVector(size_t n, int f)
    : std::vector<int>(n, f), ref_count_(0) {
  }

  /* Constructs a copy of the given vector. */
  IntVector(const IntVector& v)
    : std::vector<int>(v), ref_count_(0) {
  }

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Orderings */

/* Minimum distance between two ordered steps. */
float Orderings::threshold = 0.01f;


/* Constructs an empty ordering collection. */
Orderings::Orderings()
  : ref_count_(0) {
}


/* Constructs a copy of this ordering collection. */
Orderings::Orderings(const Orderings& o)
  : ref_count_(0) {
}


/* Deletes this ordering collection. */
Orderings::~Orderings() {
}


/* Output operator for orderings. */
std::ostream& operator<<(std::ostream& os, const Orderings& o) {
  o.print(os);
  return os;
}


/* ====================================================================== */
/* BinaryOrderings */


/* Constructs an empty ordering collection. */
BinaryOrderings::BinaryOrderings() {}


/* Constructs a copy of this ordering collection. */
BinaryOrderings::BinaryOrderings(const BinaryOrderings& o)
  : Orderings(o), before_(o.before_) {
  size_t n = before_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::register_use(before_[i]);
  }
}


/* Deletes this ordering collection. */
BinaryOrderings::~BinaryOrderings() {
  size_t n = before_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::unregister_use(before_[i]);
  }
}


/* Checks if the first step could be ordered before the second step. */
bool BinaryOrderings::possibly_before(size_t id1, StepTime t1,
				      size_t id2, StepTime t2) const {
  if (id1 == id2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return true;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return false;
  } else {
    return !before(id2, id1);
  }
}


/* Checks if the first step could be ordered after or at the same
   time as the second step. */
bool BinaryOrderings::possibly_not_before(size_t id1, StepTime t1,
					  size_t id2, StepTime t2) const {
  return possibly_after(id1, t1, id2, t2);
}


/* Checks if the first step could be ordered after the second step. */
bool BinaryOrderings::possibly_after(size_t id1, StepTime t1,
				     size_t id2, StepTime t2) const {
  if (id1 == id2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return true;
  } else {
    return !before(id1, id2);
  }
}


/* Checks if the first step could be ordered before or at the same
   time as the second step. */
bool BinaryOrderings::possibly_not_after(size_t id1, StepTime t1,
					 size_t id2, StepTime t2) const {
  return possibly_before(id1, t1, id2, t2);
}


/* Checks if the two steps are possibly concurrent. */
bool BinaryOrderings::possibly_concurrent(size_t id1, size_t id2,
					  bool& ss, bool& se,
					  bool& es, bool& ee) const {
  if (id1 == id2 || id1 == 0 || id1 == Plan::GOAL_ID
      || id2 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else {
    return ss = se = es = ee = !before(id1, id2) && !before(id2, id1);
  }
}


/* Returns the the ordering collection with the given additions. */
const BinaryOrderings*
BinaryOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
      && possibly_not_before(new_ordering.before_id(),
			     new_ordering.before_time(),
			     new_ordering.after_id(),
			     new_ordering.after_time())) {
    BinaryOrderings& orderings = *new BinaryOrderings(*this);
    std::map<size_t, BoolVector*> own_data;
    orderings.fill_transitive(own_data, new_ordering);
    return &orderings;
  } else {
    return this;
  }
}


/* Returns the the ordering collection with the given additions. */
const BinaryOrderings*
BinaryOrderings::refine(const Ordering& new_ordering,
			const Step& new_step, const PlanningGraph* pg,
			const Bindings* bindings) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    BinaryOrderings& orderings = *new BinaryOrderings(*this);
    std::map<size_t, BoolVector*> own_data;
    if (new_step.id() > before_.size() + 1) {
      if (new_step.id() > 1) {
	BoolVector* bv = new BoolVector(2*new_step.id() - 2, false);
	own_data.insert(std::make_pair(orderings.before_.size(), bv));
	orderings.before_.push_back(bv);
	BoolVector::register_use(bv);
      }
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
      orderings.fill_transitive(own_data, new_ordering);
    }
    return &orderings;
  } else {
    return this;
  }
}


/* Fills the given tables with distances for each step from the
   start step, and returns the greatest distance. */
float BinaryOrderings::schedule(std::map<size_t, float>& start_times,
				std::map<size_t, float>& end_times) const {
  float max_dist = 0.0f;
  size_t n = before_.size() + 1;
  for (size_t i = 1; i <= n; i++) {
    float ed = schedule(start_times, end_times, i);
    if (ed > max_dist) {
      max_dist = ed;
    }
  }
  return max_dist;
}


/* Returns the makespan of this ordering collection. */
float
BinaryOrderings::makespan(const std::map<std::pair<size_t,
			  StepTime::StepPoint>, float>& min_times) const {
  std::map<size_t, float> start_times, end_times;
  float max_dist = 0.0f;
  size_t n = before_.size() + 1;
  for (size_t i = 1; i <= n; i++) {
    float ed = schedule(start_times, end_times, i, min_times);
    if (ed > max_dist) {
      max_dist = ed;
    }
  }
  std::map<std::pair<size_t, StepTime::StepPoint>, float>::const_iterator md =
    min_times.find(std::make_pair(Plan::GOAL_ID, StepTime::START));
  if (md != min_times.end()) {
    if ((*md).second > max_dist) {
      max_dist = (*md).second;
    }
  }
  return max_dist;
}


/* Schedules the given instruction with the given constraints. */
float BinaryOrderings::schedule(std::map<size_t, float>& start_times,
				std::map<size_t, float>& end_times,
				size_t step_id) const {
  std::map<size_t, float>::const_iterator d = start_times.find(step_id);
  if (d != start_times.end()) {
    return (*d).second;
  } else {
    float sd = 1.0f;
    size_t n = before_.size() + 1;
    for (size_t j = 1; j <= n; j++) {
      if (step_id != j && before(j, step_id)) {
	float ed = 1.0f + schedule(start_times, end_times, j);
	if (ed > sd) {
	  sd = ed;
	}
      }
    }
    start_times.insert(std::make_pair(step_id, sd));
    end_times.insert(std::make_pair(step_id, sd));
    return sd;
  }
}


/* Schedules the given instruction with the given constraints. */
float
BinaryOrderings::schedule(std::map<size_t, float>& start_times,
			  std::map<size_t, float>& end_times, size_t step_id,
			  const std::map<std::pair<size_t,
			  StepTime::StepPoint>, float>& min_times) const {
  std::map<size_t, float>::const_iterator d = start_times.find(step_id);
  if (d != start_times.end()) {
    return (*d).second;
  } else {
    float sd = threshold;
    size_t n = before_.size() + 1;
    for (size_t j = 1; j <= n; j++) {
      if (step_id != j && before(j, step_id)) {
	float ed = threshold + schedule(start_times, end_times, j, min_times);
	if (ed > sd) {
	  sd = ed;
	}
      }
    }
    std::map<std::pair<size_t, StepTime::StepPoint>, float>::const_iterator
      md = min_times.find(std::make_pair(step_id, StepTime::START));
    if (md == min_times.end()) {
      md = min_times.find(std::make_pair(step_id, StepTime::END));
    }
    if (md != min_times.end()) {
      if ((*md).second > sd) {
	sd = (*md).second;
      }
    }
    start_times.insert(std::make_pair(step_id, sd));
    end_times.insert(std::make_pair(step_id, sd));
    return sd;
  }
}


/* Returns true iff the first step is ordered before the second step. */
bool BinaryOrderings::before(size_t id1, size_t id2) const {
  if (id1 == id2) {
    return false;
  } else if (id1 < id2) {
    return (*before_[id2 - 2])[id1 - 1];
  } else {
    return (*before_[id1 - 2])[2*id1 - 2 - id2];
  }
}


/* Orders the first step before the second step. */
void
BinaryOrderings::set_before(std::map<size_t, BoolVector*>& own_data,
			    size_t id1, size_t id2) {
  if (id1 != id2) {
    size_t i = std::max(id1, id2) - 2;
    BoolVector* bv;
    std::map<size_t, BoolVector*>::const_iterator vi = own_data.find(i);
    if (vi != own_data.end()) {
      bv = (*vi).second;
    } else {
      const BoolVector* old_bv = before_[i];
      bv = new BoolVector(*old_bv);
      BoolVector::register_use(bv);
      BoolVector::unregister_use(old_bv);
      before_[i] = bv;
      own_data.insert(std::make_pair(i, bv));
    }
    if (id1 < id2) {
      (*bv)[id1 - 1] = true;
    } else {
      (*bv)[2*id1 - 2 - id2] = true;
    }
  }
}


/* Updates the transitive closure given a new ordering constraint. */
void BinaryOrderings::fill_transitive(std::map<size_t, BoolVector*>& own_data,
				      const Ordering& ordering) {
  size_t i = ordering.before_id();
  size_t j = ordering.after_id();
  if (!before(i, j)) {
    /*
     * All steps ordered before i (and i itself) must be ordered
     * before j and all steps ordered after j.
     */
    size_t n = before_.size() + 1;
    for (size_t k = 1; k <= n; k++) {
      if ((k == i || before(k, i)) && !before(k, j)) {
	for (size_t l = 1; l <= n; l++) {
	  if ((j == l || before(j, l)) && !before(k, l)) {
	    set_before(own_data, k, l);
	  }
	}
      }
    }
  }
}


/* Prints this ordering collection on the given stream. */
void BinaryOrderings::print(std::ostream& os) const {
  os << "{";
  size_t n = before_.size() + 1;
  for (size_t i = 1; i <= n; i++) {
    for (size_t j = 1; j <= n; j++) {
      if (before(i, j)) {
	os << ' ' << i << '<' << j;
      }
    }
  }
  os << " }";
}


/* ====================================================================== */
/* TemporalOrderings */

/* Constructs an empty ordering collection. */
TemporalOrderings::TemporalOrderings()
  : goal_achievers_(NULL) {}


/* Constructs a copy of this ordering collection. */
TemporalOrderings::TemporalOrderings(const TemporalOrderings& o)
  : Orderings(o), distance_(o.distance_), goal_achievers_(o.goal_achievers_) {
  size_t n = distance_.size();
  for (size_t i = 0; i < n; i++) {
    IntVector::register_use(distance_[i]);
  }
  RCObject::ref(goal_achievers_);
}


/* Deletes this ordering collection. */
TemporalOrderings::~TemporalOrderings() {
  size_t n = distance_.size();
  for (size_t i = 0; i < n; i++) {
    IntVector::unregister_use(distance_[i]);
  }
  RCObject::destructive_deref(goal_achievers_);
}


/* Checks if the first step could be ordered before the second step. */
bool TemporalOrderings::possibly_before(size_t id1, StepTime t1,
					size_t id2, StepTime t2) const {
  if (id1 == id2 && t1 >= t2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return true;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return false;
  } else {
    int dist = distance(time_node(id1, t1), time_node(id2, t2));
    return dist > 0 || (dist == 0 && t1.rel < t2.rel);
  }
}


/* Checks if the first step could be ordered after or at the same
   time as the second step. */
bool TemporalOrderings::possibly_not_before(size_t id1, StepTime t1,
					    size_t id2, StepTime t2) const {
  if (id1 == id2 && t1 < t2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return true;
  } else {
    int dist = distance(time_node(id2, t2), time_node(id1, t1));
    return dist > 0 || (dist == 0 && t2.rel <= t1.rel);
  }
}


/* Checks if the first step could be ordered after the second step. */
bool TemporalOrderings::possibly_after(size_t id1, StepTime t1,
				       size_t id2, StepTime t2) const {
  if (id1 == id2 && t1 <= t2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return true;
  } else {
    int dist = distance(time_node(id2, t2), time_node(id1, t1));
    return dist > 0 || (dist == 0 && t2.rel < t1.rel);
  }
}


/* Checks if the first step could be ordered before or at the same
   time as the second step. */
bool TemporalOrderings::possibly_not_after(size_t id1, StepTime t1,
					   size_t id2, StepTime t2) const {
  if (id1 == id2 && t1 > t2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return true;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return false;
  } else {
    int dist = distance(time_node(id1, t1), time_node(id2, t2));
    return dist > 0 || (dist == 0 && t1.rel <= t2.rel);
  }
}


/* Checks if the two steps are possibly concurrent. */
bool TemporalOrderings::possibly_concurrent(size_t id1, size_t id2,
					    bool& ss, bool& se,
					    bool& es, bool& ee) const {
  if (id1 == id2 || id1 == 0 || id1 == Plan::GOAL_ID
      || id2 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else {
    size_t t1s = time_node(id1, StepTime::AT_START);
    size_t t1e = time_node(id1, StepTime::AT_END);
    size_t t2s = time_node(id2, StepTime::AT_START);
    size_t t2e = time_node(id2, StepTime::AT_END);
    ss = distance(t1s, t2s) >= 0 && distance(t2s, t1s) >= 0;
    se = distance(t1s, t2e) >= 0 && distance(t2e, t1s) >= 0;
    es = distance(t1e, t2s) >= 0 && distance(t2s, t1e) >= 0;
    ee = distance(t1e, t2e) >= 0 && distance(t2e, t1e) >= 0;
    return ss || se || es || ee;
  }
}


/* Returns the ordering collection with the given additions. */
const TemporalOrderings* TemporalOrderings::refine(size_t step_id,
						   float min_start,
						   float min_end) const {
  if (step_id != 0 && step_id != Plan::GOAL_ID) {
    size_t i = time_node(step_id, StepTime::AT_START);
    size_t j = time_node(step_id, StepTime::AT_END);
    int start = int(min_start/threshold + 0.5);
    int end = int(min_end/threshold + 0.5);
    if (-distance(i, 0) >= start && -distance(j, 0) >= end) {
      return this;
    } else if (distance(0, i) < start || distance(0, j) < end) {
      return NULL;
    } else {
      TemporalOrderings& orderings = *new TemporalOrderings(*this);
      std::map<size_t, IntVector*> own_data;
      if (orderings.fill_transitive(own_data, 0, i, start)
	  && orderings.fill_transitive(own_data, 0, j, end)) {
	return &orderings;
      } else {
	delete &orderings;
	return NULL;
      }
    }
  } else {
    return this;
  }
}


/* Returns the ordering collection with the given additions. */
const TemporalOrderings*
TemporalOrderings::refine(float time, const Step& new_step) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID
      && new_step.id() > distance_.size()/2) {
    int itime = int(time/threshold + 0.5);
    TemporalOrderings& orderings = *new TemporalOrderings(*this);
    IntVector* fv = new IntVector(4*new_step.id() - 2, std::numeric_limits<int>::max());
    /* Time for start of new step. */
    (*fv)[0] = itime;
    (*fv)[4*new_step.id() - 3] = -itime;
    for (size_t id = 1; id < new_step.id(); id++) {
      int t = itime - (*distance_[2*id - 1])[0];
      (*fv)[2*id - 1] = (*fv)[2*id] = t;
      (*fv)[4*new_step.id() - 2*id - 2] = -t;
      (*fv)[4*new_step.id() - 2*id - 3] = -t;
    }
    orderings.distance_.push_back(fv);
    IntVector::register_use(fv);
    fv = new IntVector(4*new_step.id(), std::numeric_limits<int>::max());
    /* Time for end of new step. */
    (*fv)[0] = itime;
    (*fv)[4*new_step.id() - 1] = -itime;
    for (size_t id = 1; id < new_step.id(); id++) {
      int t = itime - (*distance_[2*id - 1])[0];
      (*fv)[2*id - 1] = (*fv)[2*id] = t;
      (*fv)[4*new_step.id() - 2*id] = -t;
      (*fv)[4*new_step.id() - 2*id - 1] = -t;
    }
    (*fv)[2*new_step.id() - 1] = (*fv)[2*new_step.id()] = 0;
    orderings.distance_.push_back(fv);
    IntVector::register_use(fv);
    return &orderings;
  } else {
    return this;
  }
}


/* Returns the the ordering collection with the given additions. */
const TemporalOrderings*
TemporalOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
      && possibly_not_before(new_ordering.before_id(),
			     new_ordering.before_time(),
			     new_ordering.after_id(),
			     new_ordering.after_time())) {
    TemporalOrderings& orderings = *new TemporalOrderings(*this);
    std::map<size_t, IntVector*> own_data;
    size_t i = time_node(new_ordering.before_id(), new_ordering.before_time());
    size_t j = time_node(new_ordering.after_id(), new_ordering.after_time());
    int dist;
    if (new_ordering.before_time().rel < new_ordering.after_time().rel) {
      dist = 0;
    } else {
      dist = 1;
    }
    if (orderings.fill_transitive(own_data, i, j, dist)) {
      return &orderings;
    } else {
      delete &orderings;
      return NULL;
    }
  } else {
    return this;
  }
}


/* Returns the the ordering collection with the given additions. */
const TemporalOrderings*
TemporalOrderings::refine(const Ordering& new_ordering,
			  const Step& new_step, const PlanningGraph* pg,
			  const Bindings* bindings) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    TemporalOrderings& orderings = *new TemporalOrderings(*this);
    std::map<size_t, IntVector*> own_data;
    if (new_step.id() > distance_.size()/2) {
      const Value* min_v =
	dynamic_cast<const Value*>(&new_step.action().min_duration());
      if (min_v == NULL) {
	throw std::runtime_error("non-constant minimum duration");
      }
      const Value* max_v =
	dynamic_cast<const Value*>(&new_step.action().max_duration());
      if (max_v == NULL) {
	throw std::runtime_error("non-constant maximum duration");
      }
      float start_time = threshold;
      float end_time;
      if (pg != NULL) {
	HeuristicValue h, hs;
	new_step.action().condition().heuristic_value(h, hs, *pg,
						      new_step.id(), bindings);
	if (hs.makespan() > start_time) {
	  start_time = hs.makespan();
	}
	end_time = start_time + min_v->value();
	if (h.makespan() > end_time) {
	  end_time = h.makespan();
	}
      } else {
	end_time = threshold + min_v->value();
      }
      IntVector* fv = new IntVector(4*new_step.id() - 2, std::numeric_limits<int>::max());
      /* Earliest time for start of new step. */
      (*fv)[4*new_step.id() - 3] = -int(start_time/threshold + 0.5);
      own_data.insert(std::make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      IntVector::register_use(fv);
      fv = new IntVector(4*new_step.id(), std::numeric_limits<int>::max());
      /* Earliest time for end of new step. */
      (*fv)[4*new_step.id() - 1] = -int(end_time/threshold + 0.5);
      if (max_v->value() != std::numeric_limits<float>::infinity()) {
	(*fv)[2*new_step.id() - 1] = int(max_v->value()/threshold + 0.5);
      }
      (*fv)[2*new_step.id()] = -int(min_v->value()/threshold + 0.5);
      own_data.insert(std::make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      IntVector::register_use(fv);
    }
    if (new_ordering.before_id() != 0) {
      if (new_ordering.after_id() != Plan::GOAL_ID) {
	size_t i = time_node(new_ordering.before_id(),
			     new_ordering.before_time());
	size_t j = time_node(new_ordering.after_id(),
			     new_ordering.after_time());
	int dist;
	if (new_ordering.before_time().rel < new_ordering.after_time().rel) {
	  dist = 0;
	} else {
	  dist = 1;
	}
	if (orderings.fill_transitive(own_data, i, j, dist)) {
	  return &orderings;
	} else {
	  delete &orderings;
	  return NULL;
	}
      } else {
	orderings.goal_achievers_ =
	  new Chain<size_t>(new_ordering.before_id(),
			    orderings.goal_achievers_);
	RCObject::ref(orderings.goal_achievers_);
      }
    }
    return &orderings;
  } else {
    return this;
  }
}


/* Fills the given tables with distances for each step from the
   start step, and returns the greatest distance. */
float
TemporalOrderings::schedule(std::map<size_t, float>& start_times,
			    std::map<size_t, float>& end_times) const {
  float max_dist = 0.0f;
  size_t n = distance_.size()/2;
  for (size_t i = 1; i <= n; i++) {
    float sd = -distance(time_node(i, StepTime::AT_START), 0)*threshold;
    start_times.insert(std::make_pair(i, sd));
    float ed = -distance(time_node(i, StepTime::AT_END), 0)*threshold;
    end_times.insert(std::make_pair(i, ed));
    if (ed > max_dist
	&& goal_achievers_ != NULL && goal_achievers_->contains(i)) {
      max_dist = ed;
    }
  }
  return max_dist;
}


/* Returns the makespan of this ordering collection. */
float
TemporalOrderings::makespan(const std::map<std::pair<size_t,
			    StepTime::StepPoint>, float>& min_times) const {
  float max_dist = 0.0f;
  size_t n = distance_.size()/2;
  for (size_t i = 1; i <= n; i++) {
    float ed = -distance(time_node(i, StepTime::AT_END), 0)*threshold;
    if (ed > max_dist
	&& goal_achievers_ != NULL && goal_achievers_->contains(i)) {
      max_dist = ed;
    }
  }
  return max_dist;
}


/* Returns the maximum distance from the first and the second time node. */
int TemporalOrderings::distance(size_t t1, size_t t2) const {
  if (t1 == t2) {
    return 0;
  } else if (t1 < t2) {
    return (*distance_[t2 - 1])[t1];
  } else {
    return (*distance_[t1 - 1])[2*t1 - 1 - t2];
  }
}


/* Sets the maximum distance from the first and the second time node. */
void TemporalOrderings::set_distance(std::map<size_t, IntVector*>& own_data,
				     size_t t1, size_t t2, int d) {
  if (t1 != t2) {
    size_t i = std::max(t1, t2) - 1;
    IntVector* fv;
    std::map<size_t, IntVector*>::const_iterator vi = own_data.find(i);
    if (vi != own_data.end()) {
      fv = (*vi).second;
    } else {
      const IntVector* old_fv = distance_[i];
      fv = new IntVector(*old_fv);
      IntVector::register_use(fv);
      IntVector::unregister_use(old_fv);
      distance_[i] = fv;
      own_data.insert(std::make_pair(i, fv));
    }
    if (t1 < t2) {
      (*fv)[t1] = d;
    } else {
      (*fv)[2*t1 - 1 - t2] = d;
    }
  }
}


/* Updates the transitive closure given a new ordering constraint. */
bool
TemporalOrderings::fill_transitive(std::map<size_t, IntVector*>& own_data,
				   size_t i, size_t j, int dist) {
  if (distance(j, i) > -dist) {
    /*
     * Update the temporal constraints.
     *
     * Make sure that -d_ij <= d_ji always holds.
     */
    size_t n = distance_.size();
    for (size_t k = 0; k <= n; k++) {
      int d_ik = distance(i, k);
      if (d_ik < std::numeric_limits<int>::max() && distance(j, k) > d_ik - dist) {
	for (size_t l = 0; l <= n; l++) {
	  int d_lj = distance(l, j);
	  int new_d = d_ik + d_lj - dist;
	  if (d_lj < std::numeric_limits<int>::max() && distance(l, k) > new_d) {
	    set_distance(own_data, l, k, new_d);
	    if (-distance(k, l) > new_d) {
	      return false;
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Prints this ordering collection on the given stream. */
void TemporalOrderings::print(std::ostream& os) const {
  size_t n = distance_.size();
  for (size_t r = 0; r <= n; r++) {
    os << std::endl;
    for (size_t c = 0; c <= n; c++) {
      os.width(7);
      int d = distance(r, c);
      if (d < std::numeric_limits<int>::max()) {
	os << d;
      } else {
	os << "inf";
      }
    }
  }
}
