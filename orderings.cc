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
 * $Id: orderings.cc,v 3.4 2002-03-23 15:18:30 lorens Exp $
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
/* Orderings */

/* Constructs an empty ordering collection. */
Orderings::Orderings()
  : size_(0) {
#ifdef TRANSFORMATIONAL
  orderings_ = NULL;
#endif
}


/* Constructs a copy of this ordering collection. */
Orderings::Orderings(const Orderings& orderings)
  : size_(orderings.size_), id_map1_(orderings.id_map1_),
    id_map2_(orderings.id_map2_), order_(orderings.order_) {
#ifdef TRANSFORMATIONAL
  orderings_ = orderings.orderings_;
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


/* Fills the given table with distances for each step to the goal
   step, and returns the greatest distance. */
float Orderings::goal_distances(hash_map<size_t, float>& start_dist,
				hash_map<size_t, float>& end_dist) const {
  float max_dist = 0;
  for (size_t i = 0; i < size_; i++) {
    max_dist = max(max_dist, goal_distance(start_dist, end_dist, id_map2_[i]));
  }
  return max_dist;
}


/* Updates the transitive closure given new ordering constraints. */
bool Orderings::fill_transitive(const OrderingChain* orderings) {
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    if (!fill_transitive(o->head)) {
      return false;
    }
  }
  return true;
}


/* Prints this ordering collection on the given stream. */
void Orderings::print(ostream& os) const {
  os << "{";
  for (size_t i = 0; i < size_; i++) {
    for (size_t j = 0; j < size_; j++) {
      if (order_[i][j]) {
	os << ' ' << id_map2_[i] << '<' << id_map2_[j];
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
      id_map1_.insert(make_pair(step.id(), size_++));
    }
  }
  id_map2_.reserve(size_);
  order_ = vector<vector<bool> >(size_, vector<bool>(size_, false));
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  Orderings::fill_transitive(orderings);
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
    return !order_[(*j).second][(*i).second];
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
    return !order_[(*i).second][(*j).second];
  }
}


/* Computes the flexibility of this ordering collection as defined in
   "Reviving Partial Order Planning" (Nguyen & Kambhampati 2001). */
float BinaryOrderings::flexibility() const {
  if (size_ == 0) {
    return 0.0f;
  } else {
    size_t unordered = 0;
    for (size_t i = 0; i < size_; i++) {
      for (size_t j = 0; j < size_; j++) {
	if (i != j) {
	  if (!order_[i][j] && !order_[j][i]) {
	    unordered++;
	  }
	}
      }
    }
    return float(unordered)/size_;
  }
}


/* Returns the the ordering collection with the given additions. */
const Orderings* BinaryOrderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID) {
    BinaryOrderings& orderings = *(new BinaryOrderings(*this));
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(&new_ordering, orderings.orderings_);
#endif
    if (orderings.fill_transitive(new_ordering)) {
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
const Orderings* BinaryOrderings::refine(const Ordering& new_ordering,
					 const Step& new_step) const {
  BinaryOrderings& orderings = *(new BinaryOrderings(*this));
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(&new_ordering, orderings.orderings_);
#endif
  }
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      for (size_t i = 0; i < size_; i++) {
	orderings.order_[i].push_back(false);
      }
      orderings.id_map1_.insert(make_pair(new_step.id(), orderings.size_++));
      orderings.id_map2_.push_back(new_step.id());
      orderings.order_.push_back(vector<bool>(orderings.size_, false));
    }
  }
  if (orderings.fill_transitive(new_ordering)) {
    return &orderings;
  } else {
    delete &orderings;
    return NULL;
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
    for (size_t j = 0; j < size_; j++) {
      if (i != j && order_[i][j]) {
	sd = max(sd, 1.0f + goal_distance(start_dist, end_dist, id_map2_[j]));
      }
    }
    start_dist.insert(make_pair(step_id, sd));
    end_dist.insert(make_pair(step_id, sd));
    return sd;
  }
}


/* Updates the transitive closure given a new ordering constraint. */
bool BinaryOrderings::fill_transitive(const Ordering& ordering) {
  if (ordering.before_id() != 0 && ordering.after_id() != Plan::GOAL_ID) {
    size_t i = id_map1_[ordering.before_id()];
    size_t j = id_map1_[ordering.after_id()];
    if (!order_[i][j]) {
      for (size_t k = 0; k < size_; k++) {
	if (k != i && (k == j || order_[j][k]) && !order_[i][k]) {
	  for (size_t l = 0; l < size_; l++) {
	    if (l != i && l != j && l != k && order_[l][i] && !order_[l][k]) {
	      order_[l][k] = true;
	    }
	  }
	  order_[i][k] = true;
	}
      }
    }
  }
  return true;
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


/* Constructs a copy of this ordering collection. */
TemporalOrderings::TemporalOrderings(const TemporalOrderings& orderings)
  : Orderings(orderings), distance_(orderings.distance_) {
}


/* Constructs an ordering collection. */
TemporalOrderings::TemporalOrderings(const StepChain* steps,
				     const OrderingChain* orderings) {
  for (const StepChain* s = steps; s != NULL; s = s->tail) {
    const Step& step = s->head;
    if (step.id() != 0 && step.id() != Plan::GOAL_ID
	&& id_map1_.find(step.id()) == id_map1_.end()) {
      id_map1_.insert(make_pair(step.id(), 2*size_));
      size_++;
    }
  }
  id_map2_.reserve(size_);
  order_ = vector<vector<bool> >(2*size_, vector<bool>(2*size_, false));
  for (size_t i = 0; i < 2*size_; i++) {
    distance_.push_back(vector<float>());
    for (size_t j = 0; j < 2*size_; j++) {
      if (i == j) {
	distance_[i].push_back(0.0f);
      } else {
	distance_[i].push_back(INFINITY);
      }
    }
  }
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  Orderings::fill_transitive(orderings);
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
      new OrderingChain(&new_ordering, orderings.orderings_);
#endif
    if (orderings.fill_transitive(new_ordering)) {
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
  TemporalOrderings& orderings = *(new TemporalOrderings(*this));
  if (new_ordering.before_id() != 0
      && new_ordering.after_id() != Plan::GOAL_ID) {
#ifdef TRANSFORMATIONAL
    orderings.orderings_ =
      new OrderingChain(&new_ordering, orderings.orderings_);
#endif
  }
  if (new_step.id() != 0 && new_step.id() != Plan::GOAL_ID) {
    if (orderings.id_map1_.find(new_step.id()) == orderings.id_map1_.end()) {
      for (size_t i = 0; i < 2*size_; i++) {
	orderings.order_[i].push_back(false);
	orderings.order_[i].push_back(false);
	orderings.distance_[i].push_back(INFINITY);
	orderings.distance_[i].push_back(INFINITY);
      }
      orderings.id_map1_.insert(make_pair(new_step.id(), 2*size_));
      orderings.size_++;
      orderings.id_map2_.push_back(new_step.id());
      orderings.order_.push_back(vector<bool>(2*orderings.size_, false));
      orderings.order_.push_back(vector<bool>(2*orderings.size_, false));
      orderings.distance_.push_back(vector<float>(2*size_, INFINITY));
      orderings.distance_[2*size_].push_back(0.0f);
      orderings.distance_[2*size_].push_back(new_step.action()->max_duration);
      orderings.distance_.push_back(vector<float>(2*size_, INFINITY));
      orderings.distance_[2*size_+1].push_back(-new_step.action()->min_duration);
      orderings.distance_[2*size_+1].push_back(0.0f);
    }
  }
  if (orderings.fill_transitive(new_ordering)) {
    return &orderings;
  } else {
    delete &orderings;
    return NULL;
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
  for (size_t j = 0; j < 2*size_; j++) {
    if (i != j) {
      if (t == STEP_START && i + 1 == j && !order_[i][j]) {
	sd = max(sd, (-distance_[j][i] + goal_distance(start_dist, end_dist,
						       step_id, STEP_END)));
      } else if (order_[i][j]) {
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
bool TemporalOrderings::fill_transitive(const Ordering& ordering) {
  if (ordering.before_id() != 0 && ordering.after_id() != Plan::GOAL_ID) {
    size_t i = time_node(ordering.before_id(), ordering.before_time());
    size_t j = time_node(ordering.after_id(), ordering.after_time());
    bool change = false;
    if (!order_[i][j]) {
      for (size_t k = 0; k < 2*size_; k++) {
	if (k != i && (k == j || order_[j][k]) && !order_[i][k]) {
	  for (size_t l = 0; l < 2*size_; l++) {
	    if (l != i && l != j && l != k && order_[l][i] && !order_[l][k]) {
	      order_[l][k] = true;
	      if (distance_[k][l] > 0.0f) {
		distance_[k][l] = 0.0f;
		change = true;
	      }
	    }
	  }
	  order_[i][k] = true;
	  if (distance_[k][i] > 0.0f) {
	    distance_[k][i] = 0.0f;
	    change = true;
	  }
	}
      }
    }
    if (change) {
      for (size_t k = 0; k < 2*size_; k++) {
	for (size_t i = 0; i < 2*size_; i++) {
	  for (size_t j = 0; j < 2*size_; j++) {
	    float d = distance_[i][k] + distance_[k][j];
	    if (d < distance_[i][j]) {
	      distance_[i][j] = d;
	      if (order_[i][j] && d <= 0.0) {
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
