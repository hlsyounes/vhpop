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
 * $Id: orderings.cc,v 3.5 2002-03-28 18:41:55 lorens Exp $
 */
#include "orderings.h"
#include "plans.h"
#include "reasons.h"
#include "domains.h"
#include "formulas.h"
#define __USE_ISOC99 1
#include <cmath>


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
  reason_ = & reason;
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

/* Register use of the given vector. */
void BoolVector::register_use(const BoolVector* v) {
  if (v != NULL) {
    v->ref_count_++;
  }
}


/* Unregister use of the given vector. */
void BoolVector::unregister_use(const BoolVector* v) {
  if (v != NULL) {
    v->ref_count_--;
    if (v->ref_count_ == 0) {
      delete v;
    }
  }
}


/* Constructs a vector with n copies of b. */
BoolVector::BoolVector(size_t n, bool b)
  : vector<bool>(n, b), ref_count_(0) {
#ifdef DEBUG
    created_collectibles++;
#endif
}


/* Constructs a copy of the given vector. */
BoolVector::BoolVector(const BoolVector& v)
  : vector<bool>(v), ref_count_(0) {
#ifdef DEBUG
    created_collectibles++;
#endif
}


#ifdef DEBUG
/* Deletes this vector. */
BoolVector::~BoolVector() {
  deleted_collectibles++;
}
#endif


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
  : id_map1_(o.id_map1_), id_map2_(o.id_map2_), order_(o.order_) {
  size_t n = order_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::register_use(order_[i]);
  }
#ifdef TRANSFORMATIONAL
  orderings_ = parent->orderings_;
#endif
}


/* Deletes this ordering collection. */
Orderings::~Orderings() {
  size_t n = order_.size();
  for (size_t i = 0; i < n; i++) {
    BoolVector::unregister_use(order_[i]);
  }
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


/* Returns the entry at (r,c) in the matrix representing the
   transitive closure of the ordering constraints. */
bool Orderings::order(size_t r, size_t c) const {
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
void Orderings::set_order(hash_map<size_t, BoolVector*>& own_data,
			  size_t r, size_t c) {
  if (r != c) {
    size_t i = max(r, c) - 1;
    BoolVector* v;
    hash_map<size_t, BoolVector*>::const_iterator vi = own_data.find(i);
    if (vi == own_data.end()) {
      const BoolVector* old_v = order_[i];
      v = new BoolVector(*old_v);
      BoolVector::unregister_use(old_v);
      order_[i] = v;
      BoolVector::register_use(v);
      own_data.insert(make_pair(i, v));
    } else {
      v = (*vi).second;
    }
    size_t j = (r < c) ? r : 2*i - c + 1;
    (*v)[j] = true;
  }
}


/* Fills the given table with distances for each step to the goal
   step, and returns the greatest distance. */
float Orderings::goal_distances(hash_map<size_t, float>& start_dist,
				hash_map<size_t, float>& end_dist) const {
  float max_dist = 0;
  for (size_t i = 0; i < size(); i++) {
    max_dist = max(max_dist, goal_distance(start_dist, end_dist, id_map2_[i]));
  }
  return max_dist;
}


/* Prints this ordering collection on the given stream. */
void Orderings::print(ostream& os) const {
  os << "{";
  for (size_t r = 0; r < size(); r++) {
    for (size_t c = 0; c < size(); c++) {
      if (order(r, c)) {
	os << ' ' << id_map2_[r] << '<' << id_map2_[c];
      }
    }
  }
  os << " }";
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
    BoolVector* v = new BoolVector(2*i, false);
    own_data.insert(make_pair(order_.size(), v));
    order_.push_back(v);
    BoolVector::register_use(v);
  }
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    fill_transitive(own_data, o->head);
  }
}


/* Constructs a copy of this ordering collection. */
BinaryOrderings::BinaryOrderings(const BinaryOrderings& o)
  : Orderings(o) {}


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


/* Computes the flexibility of this ordering collection as defined in
   "Reviving Partial Order Planning" (Nguyen & Kambhampati 2001). */
float BinaryOrderings::flexibility() const {
  if (size() == 0) {
    return 0.0f;
  } else {
    size_t unordered = 0;
    for (size_t i = 0; i < size(); i++) {
      for (size_t j = 0; j < size(); j++) {
	if (i != j) {
	  if (!order(i, j) && !order(j, i)) {
	    unordered++;
	  }
	}
      }
    }
    return float(unordered)/size();
  }
}


/* Returns the the ordering collection with the given additions. */
const Orderings* BinaryOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID
#ifndef TRANSFORMATIONAL
      && !before(new_ordering.before_id(), new_ordering.after_id())
#endif
      ) {
    BinaryOrderings& orderings = *(new BinaryOrderings(*this));
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(new_ordering, orderings.orderings_);
#endif
    hash_map<size_t, BoolVector*> own_data;
    orderings.fill_transitive(own_data, new_ordering);
    return &orderings;
  } else {
    return this;
  }
}


/* Returns the the ordering collection with the given additions. */
const Orderings* BinaryOrderings::refine(const Ordering& new_ordering,
					 const Step& new_step) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    BinaryOrderings& orderings = *(new BinaryOrderings(*this));
    hash_map<size_t, BoolVector*> own_data;
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      orderings.id_map1_.insert(make_pair(new_step.id(), size()));
      orderings.id_map2_.push_back(new_step.id());
      size_t n = size();
      if (n > 0) {
	BoolVector* v = new BoolVector(2*n, false);
	own_data.insert(make_pair(orderings.order_.size(), v));
	orderings.order_.push_back(v);
	BoolVector::register_use(v);
      }
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
      orderings.orderings_ =
	new OrderingChain(&new_ordering, orderings.orderings_);
#endif
      orderings.fill_transitive(own_data, new_ordering);
    }
    return &orderings;
  } else {
    return this;
  }
}


/* Returns the distance of the given step to the goal step, and also
   enters it into the given distance table. */
float BinaryOrderings::goal_distance(hash_map<size_t, float>& start_dist,
				     hash_map<size_t, float>& end_dist,
				     size_t step_id, StepTime t) const {
  hash_map<size_t, float>::const_iterator d = start_dist.find(step_id);
  if (d != start_dist.end()) {
    return (*d).second;
  } else {
    float sd = 1.0f;
    size_t i = (*id_map1_.find(step_id)).second;
    for (size_t j = 0; j < size(); j++) {
      if (i != j && order(i, j)) {
	sd = max(sd, 1.0f + goal_distance(start_dist, end_dist, id_map2_[j]));
      }
    }
    start_dist.insert(make_pair(step_id, sd));
    end_dist.insert(make_pair(step_id, sd));
    return sd;
  }
}


/* Checks if the first step is ordered before the second step. */
bool BinaryOrderings::before(size_t id1, size_t id2) const {
  IdMapIter i = id_map1_.find(id1);
  IdMapIter j = id_map1_.find(id2);
  return order((*i).second, (*j).second);
}


/* Updates the transitive closure given a new ordering constraint. */
void BinaryOrderings::fill_transitive(hash_map<size_t, BoolVector*>& own_data,
				      const Ordering& ordering) {
  size_t i = id_map1_[ordering.before_id()];
  size_t j = id_map1_[ordering.after_id()];
  if (!order(i, j)) {
    size_t n = size();
    for (size_t k = 0; k < n; k++) {
      if (k != i && (k == j || order(j, k)) && !order(i, k)) {
	for (size_t l = 0; l < n; l++) {
	  if (l != i && l != j && l != k && order(l, i) && !order(l, k)) {
	    set_order(own_data, l, k);
	  }
	}
	set_order(own_data, i, k);
      }
    }
  }
}


/* ====================================================================== */
/* TemporalOrderings */

#if 0
static void print_distance_matrix(const vector<vector<float> >& d,
				  const vector<vector<bool> >& o) {
  for (size_t i = 0; i < d.size(); i++) {
    for (size_t j = 0; j < d[i].size(); j++) {
      cout << '\t' << d[i][j] << ':' << o[i][j];
    }
    cout << endl;
  }
}
#endif


/* Constructs an empty ordering collection. */
TemporalOrderings::TemporalOrderings() {}


/* Constructs an ordering collection. */
TemporalOrderings::TemporalOrderings(const StepChain* steps,
				     const OrderingChain* orderings) {
  for (const StepChain* s = steps; s != NULL; s = s->tail) {
    const Step& step = s->head;
    if (step.id() != 0 && step.id() != Plan::GOAL_ID
	&& id_map1_.find(step.id()) == id_map1_.end()) {
      id_map1_.insert(make_pair(step.id(), 2*size()));
    }
  }
  id_map2_.reserve(size());
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  size_t n = size();
  hash_map<size_t, BoolVector*> own_data;
  for (size_t i = 1; i < 2*n; i++) {
    BoolVector* v = new BoolVector(2*i, false);
    own_data.insert(make_pair(order_.size(), v));
    order_.push_back(v);
    BoolVector::register_use(v);
  }
  for (size_t i = 0; i < 2*size(); i++) {
    distance_.push_back(vector<float>());
    for (size_t j = 0; j < 2*size(); j++) {
      if (i == j) {
	distance_[i].push_back(0.0f);
      } else {
	distance_[i].push_back(INFINITY);
      }
    }
  }
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    fill_transitive(own_data, o->head);
  }
}


/* Constructs a copy of this ordering collection. */
TemporalOrderings::TemporalOrderings(const TemporalOrderings& o)
  : Orderings(o), distance_(o.distance_) {}


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
    return distance_[i][j] > 0.0f;
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
    return distance_[j][i] > 0.0f;
  }
}


/* Returns the the ordering collection with the given additions. */
const Orderings*
TemporalOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID) {
    TemporalOrderings& orderings = *(new TemporalOrderings(*this));
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(new_ordering, orderings.orderings_);
#endif
    hash_map<size_t, BoolVector*> own_data;
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
const Orderings* TemporalOrderings::refine(const Ordering& new_ordering,
					   const Step& new_step) const {
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    TemporalOrderings& orderings = *(new TemporalOrderings(*this));
    hash_map<size_t, BoolVector*> own_data;
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      orderings.id_map1_.insert(make_pair(new_step.id(), 2*size()));
      orderings.id_map2_.push_back(new_step.id());
      size_t n = size();
      if (n > 0) {
	BoolVector* v = new BoolVector(4*n, false);
	own_data.insert(make_pair(orderings.order_.size(), v));
	orderings.order_.push_back(v);
	BoolVector::register_use(v);
      }
      BoolVector* v = new BoolVector(4*n + 2, false);
      own_data.insert(make_pair(orderings.order_.size(), v));
      orderings.order_.push_back(v);
      BoolVector::register_use(v);
      for (size_t i = 0; i < 2*size(); i++) {
	orderings.distance_[i].push_back(INFINITY);
	orderings.distance_[i].push_back(INFINITY);
      }
      orderings.distance_.push_back(vector<float>(2*size(), INFINITY));
      orderings.distance_[2*size()].push_back(0.0f);
      orderings.distance_[2*size()].push_back(new_step.action()->max_duration);
      orderings.distance_.push_back(vector<float>(2*size(), INFINITY));
      orderings.distance_[2*size()+1].push_back(-new_step.action()->min_duration);
      orderings.distance_[2*size()+1].push_back(0.0f);
    }
    if (new_ordering.before_id() != 0
	&& new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
      orderings.orderings_ =
	new OrderingChain(&new_ordering, orderings.orderings_);
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


/* Returns the distance of the given step to the goal step, and also
   enters it into the given distance table. */
float TemporalOrderings::goal_distance(hash_map<size_t, float>& start_dist,
				       hash_map<size_t, float>& end_dist,
				       size_t step_id, StepTime t) const {
  if (t == STEP_START) {
    hash_map<size_t, float>::const_iterator d = start_dist.find(step_id);
    if (d != start_dist.end()) {
      return (*d).second;
    }
  } else {
    hash_map<size_t, float>::const_iterator d = end_dist.find(step_id);
    if (d != end_dist.end()) {
      return (*d).second;
    }
  }

  float sd = 1.0f;
  size_t i = time_node(step_id, t);
  for (size_t j = 0; j < 2*size(); j++) {
    if (i != j) {
      if (t == STEP_START && i + 1 == j && !order(i, j)) {
	sd = max(sd, (-distance_[j][i] + goal_distance(start_dist, end_dist,
						       step_id, STEP_END)));
      } else if (order(i, j)) {
	sd = max(sd, (min(max(1.0f, -distance_[j][i]), distance_[i][j])
		      + goal_distance(start_dist, end_dist, id_map2_[j / 2],
				      ((j % 2 == 0)
				       ? STEP_START : STEP_END))));
      }
    }
  }
  if (t == STEP_START) {
    start_dist.insert(make_pair(step_id, sd));
  } else {
    end_dist.insert(make_pair(step_id, sd));
  }
  return sd;
}


/* Updates the transitive closure given a new ordering constraint. */
bool
TemporalOrderings::fill_transitive(hash_map<size_t, BoolVector*>& own_data,
				   const Ordering& ordering) {
  size_t i = time_node(ordering.before_id(), ordering.before_time());
  size_t j = time_node(ordering.after_id(), ordering.after_time());
  bool change = false;
  if (!order(i, j)) {
    size_t n = size();
    for (size_t k = 0; k < 2*n; k++) {
      if (k != i && (k == j || order(j, k)) && !order(i, k)) {
	for (size_t l = 0; l < 2*n; l++) {
	  if (l != i && l != j && l != k && order(l, i) && !order(l, k)) {
	    set_order(own_data, l, k);
	    if (distance_[k][l] > 0.0f) {
	      distance_[k][l] = 0.0f;
	      change = true;
	    }
	  }
	}
	set_order(own_data, i, k);
	if (distance_[k][i] > 0.0f) {
	  distance_[k][i] = 0.0f;
	  change = true;
	}
      }
    }
    if (change) {
      for (size_t k = 0; k < 2*n; k++) {
	for (size_t i = 0; i < 2*n; i++) {
	  for (size_t j = 0; j < 2*n; j++) {
	    float d = distance_[i][k] + distance_[k][j];
	    if (d < distance_[i][j]) {
	      distance_[i][j] = d;
	      if (order(i, j) && d <= 0.0) {
		/* Inconsistent with ordering constraints. */
		return false;
	      }
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Returns the time node for the given step. */
size_t TemporalOrderings::time_node(size_t id, StepTime t) const {
  IdMapIter node = id_map1_.find(id);
  return (t == STEP_START) ? (*node).second : (*node).second + 1;
}
