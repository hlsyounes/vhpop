/*
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
 * $Id: orderings.cc,v 6.4 2003-08-28 15:33:17 lorens Exp $
 */
#include "orderings.h"
#include "plans.h"
#include "domains.h"
#include "debug.h"
#include "mathport.h"


/* ====================================================================== */
/* StepTime */

/* Returns the step time corresponding to the end time of the given
   effect. */
StepTime end_time(const Effect& e) {
  if (e.when() == Effect::AT_START) {
    return STEP_START;
  } else {
    return STEP_END;
  }
}


/* Returns the step time corresponding to the end time of the given
   formula time. */
StepTime end_time(FormulaTime ft) {
  if (ft == AT_START) {
    return STEP_START;
  } else {
    return STEP_END;
  }
}


/* Returns the step time corresponding to the start time of the given
   literal. */
StepTime start_time(FormulaTime ft) {
  if (ft == AT_END) {
    return STEP_END;
  } else {
    return STEP_START;
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
#ifdef DEBUG_MEMORY
    created_bool_vectors++;
#endif
  }

  /* Constructs a copy of the given vector. */
  BoolVector(const BoolVector& v)
    : std::vector<bool>(v), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_bool_vectors++;
#endif
  }

#ifdef DEBUG_MEMORY
  /* Deletes this vector. */
  ~BoolVector() {
    deleted_bool_vectors++;
  }
#endif

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* FloatVector */

/*
 * A collectible float vector.
 */
struct FloatVector : public std::vector<float> {
  /* Register use of the given vector. */
  static void register_use(const FloatVector* v) {
    if (v != NULL) {
      v->ref_count_++;
    }
  }

  /* Unregister use of the given vector. */
  static void unregister_use(const FloatVector* v) {
    if (v != NULL) {
      v->ref_count_--;
      if (v->ref_count_ == 0) {
	delete v;
      }
    }
  }

  /* Constructs a vector with n copies of b. */
  FloatVector(size_t n, float f)
    : std::vector<float>(n, f), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_float_vectors++;
#endif
  }

  /* Constructs a copy of the given vector. */
  FloatVector(const FloatVector& v)
    : std::vector<float>(v), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_float_vectors++;
#endif
  }

#ifdef DEBUG_MEMORY
  /* Deletes this vector. */
  ~FloatVector() {
    deleted_float_vectors++;
  }
#endif

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Orderings */

/* Constructs an empty ordering collection. */
Orderings::Orderings()
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_orderings++;
#endif
}


/* Constructs a copy of this ordering collection. */
Orderings::Orderings(const Orderings& o)
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_orderings++;
#endif
}


/* Deletes this ordering collection. */
Orderings::~Orderings() {
#ifdef DEBUG_MEMORY
    deleted_orderings++;
#endif
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


/* Checks if the two steps are possibly concurrent. */
bool BinaryOrderings::possibly_concurrent(size_t id1, StepTime t1,
					  size_t id2, StepTime t2) const {
  return !before(id1, id2) && !before(id2, id1);
}


/* Returns the the ordering collection with the given additions. */
const BinaryOrderings*
BinaryOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
      && possibly_after(new_ordering.before_id(), new_ordering.before_time(),
			new_ordering.after_id(), new_ordering.after_time())) {
    BinaryOrderings& orderings = *new BinaryOrderings(*this);
    std::map<size_t, BoolVector*> own_data;
    orderings.fill_transitive(own_data, new_ordering);
    return &orderings;
  } else {
    return this;
  }
}


/* Returns the the ordering collection with the given additions. */
const BinaryOrderings* BinaryOrderings::refine(const Ordering& new_ordering,
					       const Step& new_step) const {
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
    max_dist = std::max(max_dist, schedule(start_times, end_times, i));
  }
  return max_dist;
}


/* Schedules the given instruction. */
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
	sd = std::max(sd, 1.0f + schedule(start_times, end_times, j));
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
      BoolVector::unregister_use(old_bv);
      before_[i] = bv;
      BoolVector::register_use(bv);
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

/* Minimum distance between two ordered steps. */
float TemporalOrderings::threshold = 0.01f;


/* Constructs an empty ordering collection. */
TemporalOrderings::TemporalOrderings() {}


/* Constructs a copy of this ordering collection. */
TemporalOrderings::TemporalOrderings(const TemporalOrderings& o)
  : Orderings(o), distance_(o.distance_) {
  size_t n = distance_.size();
  for (size_t i = 0; i < n; i++) {
    FloatVector::register_use(distance_[i]);
  }
}


/* Deletes this ordering collection. */
TemporalOrderings::~TemporalOrderings() {
  size_t n = distance_.size();
  for (size_t i = 0; i < n; i++) {
    FloatVector::unregister_use(distance_[i]);
  }
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
    return distance(time_node(id1, t1), time_node(id2, t2)) > threshold;
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
    return distance(time_node(id2, t2), time_node(id1, t1)) > threshold;
  }
}


/* Returns the the ordering collection with the given additions. */
const TemporalOrderings*
TemporalOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
      && possibly_after(new_ordering.before_id(), new_ordering.before_time(),
			new_ordering.after_id(), new_ordering.after_time())) {
    TemporalOrderings& orderings = *new TemporalOrderings(*this);
    std::map<size_t, FloatVector*> own_data;
    if (orderings.fill_transitive(own_data, new_ordering)) {
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
			  const Step& new_step) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    TemporalOrderings& orderings = *new TemporalOrderings(*this);
    std::map<size_t, FloatVector*> own_data;
    if (new_step.id() > distance_.size()/2) {
      FloatVector* fv = new FloatVector(4*new_step.id() - 2, INFINITY);
      (*fv)[4*new_step.id() - 3] = -threshold;
      own_data.insert(std::make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      FloatVector::register_use(fv);
      fv = new FloatVector(4*new_step.id(), INFINITY);
      (*fv)[4*new_step.id() - 1] =
	-(threshold + new_step.action().min_duration());
      (*fv)[2*new_step.id() - 1] = new_step.action().max_duration();
      (*fv)[2*new_step.id()] = -new_step.action().min_duration();
      own_data.insert(std::make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      FloatVector::register_use(fv);
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
      if (orderings.fill_transitive(own_data, new_ordering)) {
	return &orderings;
      } else {
	delete &orderings;
	return NULL;
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
    max_dist = std::max(max_dist, schedule(start_times, end_times, i));
  }
  return max_dist;
}


/* Schedules the given instruction. */
float TemporalOrderings::schedule(std::map<size_t, float>& start_times,
				  std::map<size_t, float>& end_times,
				  size_t step_id) const {
  std::map<size_t, float>::const_iterator d = end_times.find(step_id);
  if (d != end_times.end()) {
    return (*d).second;
  } else {
    size_t i = time_node(step_id, STEP_START);
    start_times.insert(std::make_pair(step_id, -distance(i, 0)));
    size_t j = time_node(step_id, STEP_END);
    float ed = -distance(j, 0);
    end_times.insert(std::make_pair(step_id, ed));
    return ed;
  }
}


/* Returns the maximum distance from the first and the second time node. */
float TemporalOrderings::distance(size_t t1, size_t t2) const {
  if (t1 == t2) {
    return 0.0f;
  } else if (t1 < t2) {
    return (*distance_[t2 - 1])[t1];
  } else {
    return (*distance_[t1 - 1])[2*t1 - 1 - t2];
  }
}


/* Sets the maximum distance from the first and the second time node. */
void TemporalOrderings::set_distance(std::map<size_t, FloatVector*>& own_data,
				     size_t t1, size_t t2, float d) {
  if (t1 != t2) {
    size_t i = std::max(t1, t2) - 1;
    FloatVector* fv;
    std::map<size_t, FloatVector*>::const_iterator vi = own_data.find(i);
    if (vi != own_data.end()) {
      fv = (*vi).second;
    } else {
      const FloatVector* old_fv = distance_[i];
      fv = new FloatVector(*old_fv);
      FloatVector::unregister_use(old_fv);
      distance_[i] = fv;
      FloatVector::register_use(fv);
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
TemporalOrderings::fill_transitive(std::map<size_t, FloatVector*>& own_data,
				   const Ordering& ordering) {
  size_t i = time_node(ordering.before_id(), ordering.before_time());
  size_t j = time_node(ordering.after_id(), ordering.after_time());
  if (distance(j, i) > -threshold) {
    /*
     * Update the temporal constraints.
     *
     * Make sure that -d_ij <= d_ji always holds.
     */
    size_t n = distance_.size();
    for (size_t k = 0; k <= n; k++) {
      float d_ik = distance(i, k);
      if (!isinf(d_ik) && distance(j, k) > d_ik - threshold) {
	for (size_t l = 0; l <= n; l++) {
	  float d_lj = distance(l, j);
	  if (!isinf(d_lj) && distance(l, k) > d_ik + d_lj - threshold) {
	    set_distance(own_data, l, k, d_ik + d_lj - threshold);
	    if (-distance(k, l) > d_ik + d_lj - threshold) {
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
    if (r > 0) {
      os << std::endl;
    }
    for (size_t c = 0; c <= n; c++) {
      if (r == c) {
	os.width(6);
	os << 0;
      } else {
	size_t i = std::max(r, c) - 1;
	size_t j = (r <= c) ? r : 2*i - c + 1;
	os.width(6);
	os << distance(i, j);
      }
    }
  }
}
