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
 * $Id: orderings.cc,v 1.6 2002-01-25 18:23:06 lorens Exp $
 */
#include "orderings.h"
#include "plans.h"


/* ====================================================================== */
/* Ordering */

/* Constructs an ordering constraint. */
Ordering::Ordering(size_t before_id, size_t after_id, const Reason& reason)
  : before_id(before_id), after_id(after_id), reason(reason) {}


/* Prints this ordering on the given stream. */
void Ordering::print(ostream& os) const {
  os << before_id << '<' << after_id;
}


/* ====================================================================== */
/* Orderings */

/* Constructs an empty ordering collection. */
Orderings::Orderings()
  : orderings_(NULL), size_(0) {}


/* Constructs an ordering collection. */
Orderings::Orderings(const StepChain* steps, const OrderingChain* orderings)
  : orderings_(orderings) {
  size_ = 0;
  for (const StepChain* s = steps; s != NULL; s = s->tail) {
    const Step& step = *s->head;
    if (step.id != 0 && step.id != Plan::GOAL_ID
	&& id_map1_.find(step.id) == id_map1_.end()) {
      id_map1_.insert(make_pair(step.id, size_++));
    }
  }
  id_map2_.reserve(size_);
  order_ = vector<vector<bool> >(size_, vector<bool>(size_, false));
  for (IdMapIter i = id_map1_.begin(); i != id_map1_.end(); i++) {
    id_map2_[(*i).second] = (*i).first;
  }
  fill_transitive(orderings);
}


/* Checks if the first step is ordered before the second step. */
bool Orderings::before(size_t id1, size_t id2) const {
  if (id1 == id2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return true;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return false;
  } else {
    IdMapIter from = id_map1_.find(id1);
    IdMapIter to = id_map1_.find(id2);
    return order_[(*from).second][(*to).second];
  }
}


/* Checks if the first step is ordered after the second step. */
bool Orderings::after(size_t id1, size_t id2) const {
  if (id1 == id2) {
    return false;
  } else if (id1 == 0 || id2 == Plan::GOAL_ID) {
    return false;
  } else if (id1 == Plan::GOAL_ID || id2 == 0) {
    return true;
  } else {
    IdMapIter from = id_map1_.find(id2);
    IdMapIter to = id_map1_.find(id1);
    return order_[(*from).second][(*to).second];
  }
}


/* Checks if the first step could be ordered before the second step. */
bool Orderings::possibly_before(size_t id1, size_t id2) const {
  return id1 != id2 && !after(id1, id2);
}


/* Checks if the first step could be ordered after the second step. */
bool Orderings::possibly_after(size_t id1, size_t id2) const {
  return id1 != id2 && !before(id1, id2);
}


/* Computes the flexibility of this ordering collection as defined in
   "Reviving Partial Order Planning" (Nguyen & Kambhampati 2001). */
double Orderings::flexibility() const {
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
  return double(unordered)/size_;
}


/* Returns the the ordering collection with the given additions. */
const Orderings& Orderings::refine(const Ordering& new_ordering) const {
  if (new_ordering.before_id != 0 && new_ordering.after_id != Plan::GOAL_ID) {
    Orderings& orderings = *(new Orderings(*this));
    orderings.orderings_ =
      new OrderingChain(&new_ordering, orderings.orderings_);
    orderings.fill_transitive(new_ordering);
    return orderings;
  } else {
    return *this;
  }
}


/* Returns the the ordering collection with the given additions. */
const Orderings& Orderings::refine(const Ordering& new_ordering,
				   size_t new_step) const {
  Orderings& orderings = *(new Orderings(*this));
  if (new_ordering.before_id != 0 && new_ordering.after_id != Plan::GOAL_ID) {
    orderings.orderings_ =
      new OrderingChain(&new_ordering, orderings.orderings_);
  }
  if (new_step != 0 && new_step != Plan::GOAL_ID) {
    if (orderings.id_map1_.find(new_step) == orderings.id_map1_.end()) {
      for (size_t i = 0; i < size_; i++) {
	orderings.order_[i].push_back(false);
      }
      orderings.id_map1_.insert(make_pair(new_step, orderings.size_++));
      orderings.id_map2_.push_back(new_step);
      orderings.order_.push_back(vector<bool>(orderings.size_, false));
    }
  }
  orderings.fill_transitive(new_ordering);
  return orderings;
}


/* Prints this ordering collection on the given stream. */
void Orderings::print(ostream& os) const {
  os << "{";
  for (const OrderingChain* o = orderings_; o != NULL; o = o->tail) {
    os << ' ' << *o->head;
  }
  os << " }";
}


/* Updates the transitive closure given new ordering constraints. */
void Orderings::fill_transitive(const OrderingChain* orderings) {
  for (const OrderingChain* o = orderings; o != NULL; o = o->tail) {
    fill_transitive(*o->head);
  }
}


/* Updates the transitive closure given a new ordering constraint. */
void Orderings::fill_transitive(const Ordering& ordering) {
  if (ordering.before_id != 0 && ordering.after_id != Plan::GOAL_ID) {
    size_t i = id_map1_[ordering.before_id];
    size_t j = id_map1_[ordering.after_id];
    if (!order_[i][j]) {
      for (size_t k = 0; k < size_; k++) {
	if (k != i && (k == j || order_[j][k]) && !order_[i][k]) {
	  for (size_t l = 0; l < size_; l++) {
	    if (l != i && l !=j && l != k && order_[l][i]) {
	      order_[l][k] = true;
	    }
	  }
	  order_[i][k] = true;
	}
      }
    }
  }
}


/* Returns the distance of the given step to the goal step, and also
   enters it into the given distance table. */
size_t Orderings::goal_distance(hash_map<size_t, size_t>& dist,
				size_t step_id) const {
  hash_map<size_t, size_t>::const_iterator d = dist.find(step_id);
  if (d != dist.end()) {
    return (*d).second;
  } else {
    size_t sd = 1;
    size_t i = (*id_map1_.find(step_id)).second;
    for (size_t j = 0; j < size_; j++) {
      if (i != j && order_[i][j]) {
	sd = max(sd, 1 + goal_distance(dist, id_map2_[j]));
      }
    }
    dist.insert(make_pair(step_id, sd));
    return sd;
  }
}


/* Fills the given table with distances for each step to the goal
   step, and returns the greatest distance. */
size_t Orderings::goal_distances(hash_map<size_t, size_t>& dist) const {
  size_t max_dist = 0;
  for (size_t i = 0; i < size_; i++) {
    max_dist = max(max_dist, goal_distance(dist, id_map2_[i]));
  }
  return max_dist;
}
