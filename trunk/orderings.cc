/*
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
 * $Id: orderings.cc,v 3.18 2002-06-28 11:38:24 lorens Exp $
 */
#include "orderings.h"
#include "plans.h"
#include "reasons.h"
#include "domains.h"
#include "formulas.h"
#include "debug.h"
#define __USE_ISOC99 1
#define __USE_ISOC9X 1
#include <math.h>


/* ====================================================================== */
/* StepTime */

/* Returns the step time corresponding to the end time of the given
   effect. */
StepTime end_time(const Effect& e) {
  if (e.when == Effect::AT_START) {
    return STEP_START;
  } else {
    return STEP_END;
  }
}


/* Returns the step time corresponding to the end time of the given
   literal. */
StepTime end_time(const Literal& f) {
  if (f.when == Formula::AT_START) {
    return STEP_START;
  } else {
    return STEP_END;
  }
}


/* Returns the step time corresponding to the start time of the given
   literal. */
StepTime start_time(const Literal& f) {
  if (f.when == Formula::AT_END) {
    return STEP_END;
  } else {
    return STEP_START;
  }
}


/* ====================================================================== */
/* Ordering */

/* Constructs an ordering constraint. */
Ordering::Ordering(size_t before_id, StepTime before_time,
		   size_t after_id, StepTime after_time, const Reason& reason)
  : before_id_(before_id), before_time_(before_time),
    after_id_(after_id), after_time_(after_time) {
#ifdef TRANSFORMATIONAL
  reason_ = &reason;
  Collectible::register_use(reason_);
#endif
}


/* Constructs an ordering constraint. */
Ordering::Ordering(const Ordering& o)
  : before_id_(o.before_id_), before_time_(o.before_time_),
    after_id_(o.after_id_), after_time_(o.after_time_) {
#ifdef TRANSFORMATIONAL
  reason_ = o.reason_;
  Collectible::register_use(reason_);
#endif
}


/* Deletes this ordering constraint. */
Ordering::~Ordering() {
#ifdef TRANSFORMATIONAL
  Collectible::unregister_use(reason_);
#endif
}


/* Returns the reason. */
const Reason& Ordering::reason() const {
#ifdef TRANSFORMATIONAL
  return *reason_;
#else
  return Reason::DUMMY;
#endif
}


/* ====================================================================== */
/* BoolVector */

/*
 * A collectible bool vector.
 */
struct BoolVector : public vector<bool> {
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
    : vector<bool>(n, b), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

  /* Constructs a copy of the given vector. */
  BoolVector(const BoolVector& v)
    : vector<bool>(v), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

#ifdef DEBUG_MEMORY
  /* Deletes this vector. */
  ~BoolVector() {
    deleted_collectibles++;
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
struct FloatVector : public vector<float> {
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
    : vector<float>(n, f), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

  /* Constructs a copy of the given vector. */
  FloatVector(const FloatVector& v)
    : vector<float>(v), ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_collectibles++;
#endif
  }

#ifdef DEBUG_MEMORY
  /* Deletes this vector. */
  ~FloatVector() {
    deleted_collectibles++;
  }
#endif

private:
  /* Reference counter. */
  mutable size_t ref_count_;
};


/* ====================================================================== */
/* Orderings */

/* Constructs an empty ordering collection. */
Orderings::Orderings() {
#ifdef TRANSFORMATIONAL
  orderings_ = NULL;
#endif
}


/* Constructs a copy of this ordering collection. */
Orderings::Orderings(const Orderings& o)
  : id_map1_(o.id_map1_), id_map2_(o.id_map2_) {
#ifdef TRANSFORMATIONAL
  orderings_ = o.orderings_;
  OrderingChain::register_use(orderings_);
#endif
}


/* Deletes this ordering collection. */
Orderings::~Orderings() {
#ifdef TRANSFORMATIONAL
  OrderingChain::unregister_use(orderings_);
#endif
}


/* Returns the ordering constraints making up this collection. */
const OrderingChain* Orderings::orderings() const {
#ifdef TRANSFORMATIONAL
  return orderings_;
#else
  return NULL;
#endif
}


/* Returns the number of steps. */
size_t Orderings::size() const {
  return id_map1_.size();
}


/* Fills the given tables with distances for each step from the
   start step, and returns the greatest distance. */
float Orderings::schedule(hash_map<size_t, float>& start_times,
			  hash_map<size_t, float>& end_times) const {
  float max_dist = 0.0f;
  size_t n = size();
  for (size_t i = 0; i < n; i++) {
    max_dist = max(max_dist, schedule(start_times, end_times, id_map2_[i]));
  }
  return max_dist;
}


/* ====================================================================== */
/* BinaryOrderings */


/* Constructs an empty ordering collection. */
BinaryOrderings::BinaryOrderings() {}


/* Constructs an ordering collection. */
BinaryOrderings::BinaryOrderings(const StepChain* steps,
				 const OrderingChain* orderings) {
  for (const StepChain* s = steps; s != NULL; s = s->tail) {
    const Step& step = s->head;
    if (step.id() != 0 && step.id() != Plan::GOAL_ID
	&& id_map1_.find(step.id()) == id_map1_.end()) {
      id_map1_.insert(make_pair(step.id(), size()));
    }
  }
  id_map2_.reserve(size());
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  size_t n = size();
  hash_map<size_t, BoolVector*> own_data;
  for (size_t i = 1; i < n; i++) {
    BoolVector* bv = new BoolVector(2*i, false);
    own_data.insert(make_pair(order_.size(), bv));
    order_.push_back(bv);
    BoolVector::register_use(bv);
  }
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    fill_transitive(own_data, o->head);
  }
#ifdef TRANSFORMATIONAL
  orderings_ = orderings;
  OrderingChain::register_use(orderings);
#endif
}


/* Constructs a copy of this ordering collection. */
BinaryOrderings::BinaryOrderings(const BinaryOrderings& o)
  : Orderings(o), order_(o.order_) {
  size_t n = order_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::register_use(order_[i]);
  }
}


/* Deletes this ordering collection. */
BinaryOrderings::~BinaryOrderings() {
  size_t n = order_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::unregister_use(order_[i]);
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
    IdMapIter i = id_map1_.find(id1);
    IdMapIter j = id_map1_.find(id2);
    return !order((*j).second, (*i).second);
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
    IdMapIter i = id_map1_.find(id1);
    IdMapIter j = id_map1_.find(id2);
    return !order((*i).second, (*j).second);
  }
}


/* Checks if the two steps are possibly concurrent. */
bool BinaryOrderings::possibly_concurrent(size_t id1, StepTime t1,
					  size_t id2, StepTime t2) const {
  size_t i = (*id_map1_.find(id1)).second;
  size_t j = (*id_map1_.find(id2)).second;
  return !order(i, j) && !order(j, i);
}


/* Returns the the ordering collection with the given additions. */
const BinaryOrderings*
BinaryOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
#ifndef TRANSFORMATIONAL
      && possibly_after(new_ordering.before_id(), new_ordering.before_time(),
			new_ordering.after_id(), new_ordering.after_time())
#endif
      ) {
    BinaryOrderings& orderings = *(new BinaryOrderings(*this));
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(new_ordering, orderings.orderings_);
    OrderingChain::register_use(orderings.orderings_);
    OrderingChain::unregister_use(orderings.orderings_->tail);
#endif
    hash_map<size_t, BoolVector*> own_data;
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
    BinaryOrderings& orderings = *(new BinaryOrderings(*this));
    hash_map<size_t, BoolVector*> own_data;
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      orderings.id_map1_.insert(make_pair(new_step.id(), size()));
      orderings.id_map2_.push_back(new_step.id());
      size_t n = size();
      if (n > 0) {
	BoolVector* bv = new BoolVector(2*n, false);
	own_data.insert(make_pair(orderings.order_.size(), bv));
	orderings.order_.push_back(bv);
	BoolVector::register_use(bv);
      }
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
      orderings.orderings_ =
	new OrderingChain(&new_ordering, orderings.orderings_);
      OrderingChain::register_use(orderings.orderings_);
      OrderingChain::unregister_use(orderings.orderings_->tail);
#endif
      orderings.fill_transitive(own_data, new_ordering);
    }
    return &orderings;
  } else {
    return this;
  }
}


/* Schedules the given instruction. */
float BinaryOrderings::schedule(hash_map<size_t, float>& start_times,
				hash_map<size_t, float>& end_times,
				size_t step_id, StepTime t) const {
  hash_map<size_t, float>::const_iterator d = start_times.find(step_id);
  if (d != start_times.end()) {
    return (*d).second;
  } else {
    float sd = 1.0f;
    size_t i = (*id_map1_.find(step_id)).second;
    size_t n = size();
    for (size_t j = 0; j < n; j++) {
      if (i != j && order(j, i)) {
	sd = max(sd, 1.0f + schedule(start_times, end_times, id_map2_[j]));
      }
    }
    start_times.insert(make_pair(step_id, sd));
    end_times.insert(make_pair(step_id, sd));
    return sd;
  }
}


/* Prints this ordering collection on the given stream. */
void BinaryOrderings::print(ostream& os) const {
  os << "{";
  size_t n = size();
  for (size_t r = 0; r < n; r++) {
    for (size_t c = 0; c < n; c++) {
      if (order(r, c)) {
	os << ' ' << id_map2_[r] << '<' << id_map2_[c];
      }
    }
  }
  os << " }";
}


/* Returns the entry at (r,c) in the matrix representing the
   transitive closure of the ordering constraints. */
bool BinaryOrderings::order(size_t r, size_t c) const {
  if (r != c) {
    size_t i = max(r, c) - 1;
    size_t j = (r < c) ? r : 2*i - c + 1;
    return (*order_[i])[j];
  } else {
    return false;
  }
}


/* Sets the entry at (r,c) to true in the matrix representing the
   transitive closure of the ordering constraints. */
void BinaryOrderings::set_order(hash_map<size_t, BoolVector*>& own_data,
				size_t r, size_t c) {
  if (r != c) {
    size_t i = max(r, c) - 1;
    BoolVector* bv;
    hash_map<size_t, BoolVector*>::const_iterator vi = own_data.find(i);
    if (vi == own_data.end()) {
      const BoolVector* old_bv = order_[i];
      bv = new BoolVector(*old_bv);
      BoolVector::unregister_use(old_bv);
      order_[i] = bv;
      BoolVector::register_use(bv);
      own_data.insert(make_pair(i, bv));
    } else {
      bv = (*vi).second;
    }
    size_t j = (r < c) ? r : 2*i - c + 1;
    (*bv)[j] = true;
  }
}


/* Updates the transitive closure given a new ordering constraint. */
void BinaryOrderings::fill_transitive(hash_map<size_t, BoolVector*>& own_data,
				      const Ordering& ordering) {
  size_t i = id_map1_[ordering.before_id()];
  size_t j = id_map1_[ordering.after_id()];
  if (!order(i, j)) {
    /*
     * All steps ordered before i (and i itself) must be ordered
     * before j and all steps ordered after j.
     */
    size_t n = size();
    for (size_t k = 0; k < n; k++) {
      if ((k == i || order(k, i)) && !order(k, j)) {
	for (size_t l = 0; l < n; l++) {
	  if ((j == l || order(j, l)) && !order(k, l)) {
	    set_order(own_data, k, l);
	  }
	}
      }
    }
  }
}


/* ====================================================================== */
/* TemporalOrderings */

/* Minimum distance between two ordered steps. */
float TemporalOrderings::threshold = 1.0f;


/* Constructs an empty ordering collection. */
TemporalOrderings::TemporalOrderings() {}


/* Constructs an ordering collection. */
TemporalOrderings::TemporalOrderings(const StepChain* steps,
				     const OrderingChain* orderings) {
  hash_map<size_t, FloatVector*> own_data;
  for (const StepChain* s = steps; s != NULL; s = s->tail) {
    const Step& step = s->head;
    if (step.id() != 0 && step.id() != Plan::GOAL_ID
	&& id_map1_.find(step.id()) == id_map1_.end()) {
      int n = size();
      id_map1_.insert(make_pair(step.id(), 2*n));
      FloatVector* fv = new FloatVector(4*n + 2, INFINITY);
      (*fv)[4*n + 1] = -threshold;
      own_data.insert(make_pair(distance_.size(), fv));
      distance_.push_back(fv);
      FloatVector::register_use(fv);
      fv = new FloatVector(4*n + 4, INFINITY);
      (*fv)[4*n + 3] = -(threshold + step.action()->min_duration);
      (*fv)[2*n + 1] = step.action()->max_duration;
      (*fv)[2*n + 2] = -step.action()->min_duration;
      own_data.insert(make_pair(distance_.size(), fv));
      distance_.push_back(fv);
      FloatVector::register_use(fv);
    }
  }
  id_map2_.reserve(size());
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    fill_transitive(own_data, o->head);
  }
#ifdef TRANSFORMATIONAL
  orderings_ = orderings;
  OrderingChain::register_use(orderings);
#endif
}


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
    size_t i = time_node(id1, t1);
    size_t j = time_node(id2, t2);
    return distance(i + 1, j + 1) > threshold;
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
    size_t i = time_node(id1, t1);
    size_t j = time_node(id2, t2);
    return distance(j + 1, i + 1) > threshold;
  }
}


/* Returns the the ordering collection with the given additions. */
const TemporalOrderings*
TemporalOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
#ifndef TRANSFORMATIONAL
      && possibly_after(new_ordering.before_id(), new_ordering.before_time(),
			new_ordering.after_id(), new_ordering.after_time())
#endif
      ) {
    TemporalOrderings& orderings = *(new TemporalOrderings(*this));
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(new_ordering, orderings.orderings_);
    OrderingChain::register_use(orderings.orderings_);
    OrderingChain::unregister_use(orderings.orderings_->tail);
#endif
    hash_map<size_t, FloatVector*> own_data;
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
    TemporalOrderings& orderings = *(new TemporalOrderings(*this));
    hash_map<size_t, FloatVector*> own_data;
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      orderings.id_map1_.insert(make_pair(new_step.id(), 2*size()));
      orderings.id_map2_.push_back(new_step.id());
      size_t n = size();
      FloatVector* fv = new FloatVector(4*n + 2, INFINITY);
      (*fv)[4*n + 1] = -threshold;
      own_data.insert(make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      FloatVector::register_use(fv);
      fv = new FloatVector(4*n + 4, INFINITY);
      (*fv)[4*n + 3] = -(threshold + new_step.action()->min_duration);
      (*fv)[2*n + 1] = new_step.action()->max_duration;
      (*fv)[2*n + 2] = -new_step.action()->min_duration;
      own_data.insert(make_pair(orderings.distance_.size(), fv));
      orderings.distance_.push_back(fv);
      FloatVector::register_use(fv);
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
      orderings.orderings_ =
	new OrderingChain(&new_ordering, orderings.orderings_);
      OrderingChain::register_use(orderings.orderings_);
      OrderingChain::unregister_use(orderings.orderings_->tail);
#endif
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


/* Schedules the given instruction. */
float TemporalOrderings::schedule(hash_map<size_t, float>& start_times,
				  hash_map<size_t, float>& end_times,
				  size_t step_id, StepTime t) const {
  if (t == STEP_START) {
    hash_map<size_t, float>::const_iterator d = end_times.find(step_id);
    if (d != end_times.end()) {
      return (*d).second;
    }
    size_t i = time_node(step_id, t);
    start_times.insert(make_pair(step_id, -distance(i + 1, 0)));
    return schedule(start_times, end_times, step_id, STEP_END);
  } else {
    size_t i = time_node(step_id, t);
    float d = -distance(i + 1, 0);
    end_times.insert(make_pair(step_id, d));
    return d;
  }
}


/* Prints this ordering collection on the given stream. */
void TemporalOrderings::print(ostream& os) const {
  size_t n = distance_.size();
  for (size_t r = 0; r < n + 1; r++) {
    if (r > 0) {
      os << endl;
    }
    for (size_t c = 0; c < n + 1; c++) {
      if (r == c) {
	os.width(6);
	os << 0;
      } else {
	size_t i = max(r, c) - 1;
	size_t j = (r <= c) ? r : 2*i - c + 1;
	os.width(6);
	os << (*distance_[i])[j];
      }
    }
  }
}


/* Returns the entry at (r,c) in the matrix representing the minimal
   network for the ordering constraints. */
float TemporalOrderings::distance(size_t r, size_t c) const {
  if (r != c) {
    size_t i = max(r, c) - 1;
    size_t j = (r <= c) ? r : 2*i - c + 1;
    return (*distance_[i])[j];
  } else {
    return 0.0f;
  }
}


/* Sets the entry at (r,c) in the matrix representing the minimal
   network for the ordering constraints. */
void TemporalOrderings::set_distance(hash_map<size_t, FloatVector*>& own_data,
				     size_t r, size_t c, float d) {
  if (r != c) {
    size_t i = max(r, c) - 1;
    FloatVector* fv;
    hash_map<size_t, FloatVector*>::const_iterator vi = own_data.find(i);
    if (vi == own_data.end()) {
      const FloatVector* old_fv = distance_[i];
      fv = new FloatVector(*old_fv);
      FloatVector::unregister_use(old_fv);
      distance_[i] = fv;
      FloatVector::register_use(fv);
      own_data.insert(make_pair(i, fv));
    } else {
      fv = (*vi).second;
    }
    size_t j = (r <= c) ? r : 2*i - c + 1;
    (*fv)[j] = d;
  }
}


/* Returns the time node for the given step. */
size_t TemporalOrderings::time_node(size_t id, StepTime t) const {
  IdMapIter node = id_map1_.find(id);
  return (t == STEP_START) ? (*node).second : (*node).second + 1;
}


/* Updates the transitive closure given a new ordering constraint. */
bool
TemporalOrderings::fill_transitive(hash_map<size_t, FloatVector*>& own_data,
				   const Ordering& ordering) {
  size_t i = time_node(ordering.before_id(), ordering.before_time());
  size_t j = time_node(ordering.after_id(), ordering.after_time());
  if (distance(j + 1, i + 1) > -threshold) {
    /*
     * Update the temporal constraints.
     *
     * Make sure that -d_ij <= d_ji always holds.
     */
    size_t n = size();
    for (size_t k = 0; k < 2*n + 1; k++) {
      float d_ik = distance(i + 1, k);
      if (!isinf(d_ik) && distance(j + 1, k) > d_ik - threshold) {
	for (size_t l = 0; l < 2*n + 1; l++) {
	  float d_lj = distance(l, j + 1);
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
