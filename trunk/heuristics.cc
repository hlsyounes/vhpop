/*
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: heuristics.cc,v 6.10 2003-09-09 23:19:15 lorens Exp $
 */
#include "heuristics.h"
#include "plans.h"
#include "bindings.h"
#include "orderings.h"
#include "flaws.h"
#include "problems.h"
#include "domains.h"
#include "debug.h"
#include "mathport.h"
#include <typeinfo>
#include <set>
#include <utility>
#include <climits>


/* Generates a random number in the interval [0,1). */
static double rand01ex() {
  return rand()/(RAND_MAX + 1.0);
}


/* Returns the sum of two integers, avoiding overflow. */
static int sum(int n, int m) {
  return (INT_MAX - n > m) ? n + m : INT_MAX;
}


/* Computes the heuristic value of the given formula. */
static HeuristicValue
formula_value(const Formula& formula, FormulaTime when, size_t step_id,
	      const Plan& plan, const Domain& domain, const PlanningGraph& pg,
	      bool reuse = false) {
  const Bindings* bindings = plan.bindings();
  if (reuse) {
    const Literal* literal = dynamic_cast<const Literal*>(&formula);
    if (literal != NULL) {
      StepTime gt = start_time(when);
      if (!domain.predicates().static_predicate(literal->predicate())) {
	for (const Chain<Step>* sc = plan.steps(); sc != NULL; sc = sc->tail) {
	  const Step& step = sc->head;
	  if (step.id() != 0
	      && plan.orderings().possibly_before(step.id(), STEP_START,
						  step_id, gt)) {
	    const EffectList& effs = step.action().effects();
	    for (EffectList::const_iterator ei = effs.begin();
		 ei != effs.end(); ei++) {
	      const Effect& e = **ei;
	      StepTime et = end_time(e);
	      if (plan.orderings().possibly_before(step.id(), et,
						   step_id, gt)) {
		if (typeid(*literal) == typeid(e.literal())) {
		  if ((bindings != NULL
		       && bindings->unify(*literal, step_id,
					  e.literal(), step.id(),
					  &pg.problem()))
		      || (bindings == NULL && literal == &e.literal())) {
		    return HeuristicValue::ZERO_COST_UNIT_WORK;
		  }
		}
	      }
	    }
	  }
	}
      }
    } else {
      const Disjunction* disj = dynamic_cast<const Disjunction*>(&formula);
      if (disj != NULL) {
	HeuristicValue h = HeuristicValue::INFINITE;
	for (FormulaList::const_iterator fi = disj->disjuncts().begin();
	     fi != disj->disjuncts().end(); fi++) {
	  h = min(h, formula_value(**fi, when, step_id,
				   plan, domain, pg, true));
	}
	return h;
      } else {
	const Conjunction* conj = dynamic_cast<const Conjunction*>(&formula);
	if (conj != NULL) {
	  HeuristicValue h = HeuristicValue::ZERO;
	  for (FormulaList::const_iterator fi = conj->conjuncts().begin();
	       fi != conj->conjuncts().end(); fi++) {
	    h += formula_value(**fi, when, step_id, plan, domain, pg, true);
	  }
	  return h;
	} else {
	  const Exists* exists = dynamic_cast<const Exists*>(&formula);
	  if (exists != NULL) {
	    return formula_value(exists->body(), when, step_id,
				 plan, domain, pg, true);
	  } else {
	    const Forall* forall = dynamic_cast<const Forall*>(&formula);
	    if (forall != NULL) {
	      return formula_value(forall->universal_base(SubstitutionMap(),
							  pg.problem()),
				   when, step_id, plan, domain, pg, true);
	    }
	  }
	}
      }
    }
  }
  HeuristicValue h;
  formula.heuristic_value(h, pg, step_id, bindings);
  return h;
}


/* ====================================================================== */
/* GroundActionSet */

/*
 * Set of ground actions.
 */
struct GroundActionSet : public std::set<const GroundAction*> {
};


/* ====================================================================== */
/* HeuristicValue */

/* A zero heuristic value. */
const HeuristicValue HeuristicValue::ZERO =
HeuristicValue(0, 0, Orderings::threshold);
/* A zero cost, unit work, heuristic value. */
const HeuristicValue HeuristicValue::ZERO_COST_UNIT_WORK =
HeuristicValue(0, 1, Orderings::threshold);
/* An infinite heuristic value. */
const HeuristicValue
HeuristicValue::INFINITE = HeuristicValue(INT_MAX, INT_MAX, INFINITY);


/* Checks if this heuristic value is zero. */
bool HeuristicValue::zero() const {
  return add_cost() == 0;
}


/* Checks if this heuristic value is infinite. */
bool HeuristicValue::infinite() const {
  return isinf(makespan());
}


/* Adds the given heuristic value to this heuristic value. */
HeuristicValue& HeuristicValue::operator+=(const HeuristicValue& v) {
  add_cost_ = sum(add_cost(), v.add_cost());
  add_work_ = sum(add_work(), v.add_work());
  if (makespan() < v.makespan()) {
    makespan_ = v.makespan();
  }
  return *this;
}


/* Increments the cost of this heuristic value. */
void HeuristicValue::increment_cost() {
  add_cost_ = sum(add_cost(), 1);
}


/* Increments the work of this heuristic value. */
void HeuristicValue::increment_work() {
  add_work_ = sum(add_work(), 1);
}


/* Increases the makespan of this heuristic value. */
void HeuristicValue::increase_makespan(float x) {
  makespan_ += x;
}


/* Equality operator for heuristic values. */
bool operator==(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1.add_cost() == v2.add_cost() && v1.add_work() == v2.add_work()
	  && v1.makespan() == v2.makespan());
}


/* Inequality operator for heuristic values. */
bool operator!=(const HeuristicValue& v1, const HeuristicValue& v2) {
  return !(v1 == v2);
}


/* Less than operator for heuristic values. */
bool operator<(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1 <= v2 && v1 != v2);
}


/* Greater than operator for heuristic values. */
bool operator>(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1 >= v2 && v1 != v2);
}


/* Less than or equal to operator for heuristic values. */
bool operator<=(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1.add_cost() <= v2.add_cost() && v1.add_work() <= v2.add_work()
	  && v1.makespan() <= v2.makespan());
}


/* Greater than or equal to operator for heuristic values. */
bool operator>=(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1.add_cost() >= v2.add_cost() && v1.add_work() >= v2.add_work()
	  && v1.makespan() >= v2.makespan());
}


/* Returns the componentwise minimum heuristic value, given two
   heuristic values. */
HeuristicValue min(const HeuristicValue& v1, const HeuristicValue& v2) {
  int add_cost, add_work;
  if (v1.add_cost() == v2.add_cost()) {
    add_cost = v1.add_cost();
    add_work = std::min(v1.add_work(), v2.add_work());
  } else if (v1.add_cost() < v2.add_cost()) {
    add_cost = v1.add_cost();
    add_work = v1.add_work();
  } else {
    add_cost = v2.add_cost();
    add_work = v2.add_work();
  }
  return HeuristicValue(add_cost, add_work, 
			std::min(v1.makespan(), v2.makespan()));
}


/* Output operator for heuristic values. */
std::ostream& operator<<(std::ostream& os, const HeuristicValue& v) {
  os << "ADD<" << v.add_cost() << ',' << v.add_work() << '>'
     << " MS<" << v.makespan() << '>';
  return os;
}


/* ====================================================================== */
/* Heuristic evaluation functions for formulas. */

/* Returns the heuristic value of this formula. */
void Constant::heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const {
  h = HeuristicValue::ZERO;
}


/* Returns the heuristic value of this formula. */
void Atom::heuristic_value(HeuristicValue& h,
			   const PlanningGraph& pg, size_t step_id,
			   const Bindings* b) const {
  h = pg.heuristic_value(*this, step_id, b);
}


/* Returns the heuristic value of this formula. */
void Negation::heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const {
  h = pg.heuristic_value(*this, step_id, b);
}


/* Returns the heuristic vaue of this formula. */
void Equality::heuristic_value(HeuristicValue& h,
			       const PlanningGraph& pg, size_t step_id,
			       const Bindings* b) const {
  if (b == NULL) {
    h = HeuristicValue::ZERO;
  } else if (b->consistent_with(*this, step_id)) {
    h = HeuristicValue::ZERO;
  } else {
    h = HeuristicValue::INFINITE;
  }
}


/* Returns the heuristic value of this formula. */
void Inequality::heuristic_value(HeuristicValue& h,
				 const PlanningGraph& pg, size_t step_id,
				 const Bindings* b) const {
  if (b == NULL) {
    h = HeuristicValue::ZERO;
  } else if (b->consistent_with(*this, step_id)) {
    h = HeuristicValue::ZERO;
  } else {
    h = HeuristicValue::INFINITE;
  }
}


/* Returns the heuristic value of this formula. */
void Conjunction::heuristic_value(HeuristicValue& h,
				  const PlanningGraph& pg, size_t step_id,
				  const Bindings* b) const {
  h = HeuristicValue::ZERO;
  for (FormulaList::const_iterator fi = conjuncts().begin();
       fi != conjuncts().end() && !h.infinite(); fi++) {
    HeuristicValue hi;
    (*fi)->heuristic_value(hi, pg, step_id, b);
    h += hi;
  }
}


/* Returns the heuristic value of this formula. */
void Disjunction::heuristic_value(HeuristicValue& h,
				  const PlanningGraph& pg, size_t step_id,
				  const Bindings* b) const {
  h = HeuristicValue::INFINITE;
  for (FormulaList::const_iterator fi = disjuncts().begin();
       fi != disjuncts().end() && !h.zero(); fi++) {
    HeuristicValue hi;
    (*fi)->heuristic_value(hi, pg, step_id, b);
    h = min(h, hi);
  }
}


/* Returns the heuristic value of this formula. */
void Exists::heuristic_value(HeuristicValue& h,
			     const PlanningGraph& pg, size_t step_id,
			     const Bindings* b) const {
  body().heuristic_value(h, pg, step_id, b);
}


/* Returns the heuristic value of this formula. */
void Forall::heuristic_value(HeuristicValue& h,
			     const PlanningGraph& pg, size_t step_id,
			     const Bindings* b) const {
  const Formula& f = universal_base(SubstitutionMap(), pg.problem());
  f.heuristic_value(h, pg, step_id, b);
}


/* Returns the heuristic value of this condition. */
void Condition::heuristic_value(HeuristicValue& h, HeuristicValue& hs,
				const PlanningGraph& pg, size_t step_id,
				const Bindings* b) const {
  at_start().heuristic_value(hs, pg, step_id, b);
  over_all().heuristic_value(h, pg, step_id, b);
  hs += h;
  at_end().heuristic_value(h, pg, step_id, b);
  h += hs;
}


/* ====================================================================== */
/* PlanningGraph */

/* Constructs a planning graph. */
PlanningGraph::PlanningGraph(const Problem& problem, bool domain_constraints)
  : problem_(&problem) {
  /*
   * Find all consistent action instantiations.
   */
  GroundActionList actions;
  problem.instantiated_actions(actions);
  if (verbosity > 0) {
    std::cerr << std::endl << "Instantiated actions: " << actions.size()
	      << std::endl;
  }

  /*
   * Add initial conditions at level 0.
   */
  const GroundAction& ia = problem.init_action();
  for (EffectList::const_iterator ei = ia.effects().begin();
       ei != ia.effects().end(); ei++) {
    const Atom& atom = dynamic_cast<const Atom&>((*ei)->literal());
    achievers_[&atom].insert(std::make_pair(&ia, *ei));
    if (problem.domain().predicates().static_predicate(atom.predicate())) {
      atom_values_.insert(std::make_pair(&atom, HeuristicValue::ZERO));
    } else {
      atom_values_.insert(std::make_pair(&atom,
					 HeuristicValue::ZERO_COST_UNIT_WORK));
    }
  }

  /*
   * Generate the rest of the levels until no change occurs.
   */
  bool changed;
  int level = 0;
  /*
   * Keep track of both applicable and useful actions.  When planning
   * with durative actions, it is possible that there are useful
   * actions that are not applicable.  At the end, we make sure to
   * create action domain constraints only for actions that are both
   * applicable and useful.
   */
  GroundActionSet applicable_actions;
  GroundActionSet useful_actions;
  do {
    if (verbosity > 3) {
      /*
       * Print literal values at this level.
       */
      std::cerr << "Literal values at level " << level << ":" << std::endl;
      for (AtomValueMap::const_iterator vi = atom_values_.begin();
	   vi != atom_values_.end(); vi++) {
	std::cerr << "  ";
	(*vi).first->print(std::cerr, problem.domain().predicates(),
			   problem.terms(), 0, Bindings::EMPTY);
	std::cerr << " -- " << (*vi).second << std::endl;
      }
      for (AtomValueMap::const_iterator vi = negation_values_.begin();
	   vi != negation_values_.end(); vi++) {
	std::cerr << "  (not ";
	(*vi).first->print(std::cerr, problem.domain().predicates(),
			   problem.terms(), 0, Bindings::EMPTY);
	std::cerr << ") -- " << (*vi).second << std::endl;
      }
    }
    level++;
    changed = false;

    /*
     * Find applicable actions at current level and add effects to the
     * next level.
     */
    AtomValueMap new_atom_values;
    AtomValueMap new_negation_values;
    for (GroundActionList::const_iterator ai = actions.begin();
	 ai != actions.end(); ai++) {
      const GroundAction& action = **ai;
      HeuristicValue pre_value;
      HeuristicValue start_value;
      action.condition().heuristic_value(pre_value, start_value, *this, 0);
      if (!start_value.infinite()) {
	/* Precondition is achievable at this level. */
	if (!pre_value.infinite()
	    && applicable_actions.find(&action) == applicable_actions.end()) {
	  /* First time this action is applicable. */
	  applicable_actions.insert(&action);
	}
	for (EffectList::const_iterator ei = action.effects().begin();
	     ei != action.effects().end(); ei++) {
	  const Effect& effect = **ei;
	  if (effect.when() == Effect::AT_END && pre_value.infinite()) {
	    continue;
	  }
	  HeuristicValue cond_value, cond_value_start;
	  effect.condition().heuristic_value(cond_value, cond_value_start,
					     *this, 0);
	  if (!cond_value.infinite()
	      && !effect.link_condition().contradiction()) {
	    /* Effect condition is achievable at this level. */
	    if (effect.when() == Effect::AT_START) {
	      cond_value += start_value;
	    } else {
	      cond_value += pre_value;
	    }
	    cond_value.increment_cost();
	    const Value* min_v =
	      dynamic_cast<const Value*>(&action.min_duration());
	    if (min_v == NULL) {
	      throw Exception("non-constant minimum duration");
	    }
	    cond_value.increase_makespan(Orderings::threshold
					 + min_v->value());

	    /*
	     * Update heuristic values of literal added by effect.
	     */
	    const Literal& literal = effect.literal();
	    if (!find(achievers_, literal, action, effect)) {
	      if (!pre_value.infinite()) {
		achievers_[&literal].insert(std::make_pair(&action, &effect));
	      }
	      if (useful_actions.find(&action) == useful_actions.end()) {
		useful_actions.insert(&action);
	      }
	      if (verbosity > 4) {
		std::cerr << "  ";
		action.print(std::cerr, problem.terms(), 0, Bindings::EMPTY);
		std::cerr << " achieves ";
		literal.print(std::cerr, problem.domain().predicates(),
			      problem.terms(), 0, Bindings::EMPTY);
		std::cerr << " with ";
		effect.print(std::cerr, problem.domain().predicates(),
			     problem.terms());
		std::cerr << std::endl;
	      }
	    }
	    const Atom* atom = dynamic_cast<const Atom*>(&literal);
	    if (atom != NULL) {
	      AtomValueMap::const_iterator vi = new_atom_values.find(atom);
	      if (vi == new_atom_values.end()) {
		vi = atom_values_.find(atom);
		if (vi == atom_values_.end()) {
		  /* First level this atom is achieved. */
		  HeuristicValue new_value = cond_value;
		  new_value.increment_work();
		  new_atom_values.insert(std::make_pair(atom, new_value));
		  changed = true;
		  continue;
		}
	      }
	      /* This atom has been achieved earlier. */
	      HeuristicValue old_value = (*vi).second;
	      HeuristicValue new_value = cond_value;
	      new_value.increment_work();
	      new_value = min(new_value, old_value);
	      if (new_value < old_value) {
		new_atom_values[atom] = new_value;
		changed = true;
	      }
	    } else {
	      const Negation& negation =
		dynamic_cast<const Negation&>(literal);
	      AtomValueMap::const_iterator vi =
		new_negation_values.find(&negation.atom());
	      if (vi == new_negation_values.end()) {
		vi = negation_values_.find(&negation.atom());
		if (vi == negation_values_.end()) {
		  if (heuristic_value(negation.atom(), 0).zero()) {
		    /* First level this negated atom is achieved. */
		    HeuristicValue new_value = cond_value;
		    new_value.increment_work();
		    new_negation_values.insert(std::make_pair(&negation.atom(),
							      new_value));
		    changed = true;
		    continue;
		  } else {
		    /* Closed world assumption. */
		    continue;
		  }
		}
	      }
	      /* This negated atom has been achieved earlier. */
	      HeuristicValue old_value = (*vi).second;
	      HeuristicValue new_value = cond_value;
	      new_value.increment_work();
	      new_value = min(new_value, old_value);
	      if (new_value < old_value) {
		new_negation_values[&negation.atom()] = new_value;
		changed = true;
	      }
	    }
	  }
	}
      }
    }

    /*
     * Add achieved atoms to previously achieved atoms.
     */
    for (AtomValueMap::const_iterator vi = new_atom_values.begin();
	 vi != new_atom_values.end(); vi++) {
      atom_values_[(*vi).first] = (*vi).second;
    }
    /*
     * Add achieved negated atoms to previously achieved negated atoms.
     */
    for (AtomValueMap::const_iterator vi = new_negation_values.begin();
	 vi != new_negation_values.end(); vi++) {
      negation_values_[(*vi).first] = (*vi).second;
    }
  } while (changed);

  /*
   * Map predicates to achievable ground atoms.
   */
  for (AtomValueMap::const_iterator vi = atom_values_.begin();
       vi != atom_values_.end(); vi++) {
    const Atom& atom = *(*vi).first;
    predicate_atoms_.insert(std::make_pair(atom.predicate(), &atom));
  }

  /*
   * Map predicates to achievable negated ground atoms.
   */
  for (AtomValueMap::const_iterator vi = negation_values_.begin();
       vi != negation_values_.end(); vi++) {
    const Atom& atom = *(*vi).first;
    predicate_negations_.insert(std::make_pair(atom.predicate(), &atom));
  }

  /*
   * Collect actions that are both applicable and useful.  Create
   * actions domains constraints for these actions, if called for.
   */
  GroundActionSet good_actions;
  if (verbosity > 1 || domain_constraints) {
    for (GroundActionSet::const_iterator ai = applicable_actions.begin();
	 ai != applicable_actions.end(); ai++) {
      const GroundAction& action = **ai;
      if (useful_actions.find(&action) != useful_actions.end()) {
	good_actions.insert(&action);
	if (domain_constraints && !action.arguments().empty()) {
	  ActionDomainMap::const_iterator di =
	    action_domains_.find(action.name());
	  if (di == action_domains_.end()) {
	    ActionDomain* domain = new ActionDomain(action.arguments());
	    ActionDomain::register_use(domain);
	    action_domains_.insert(std::make_pair(action.name(), domain));
	  } else {
	    (*di).second->add(action.arguments());
	  }
	}
      }
    }
  }

  if (verbosity > 0) {
    std::cerr << "Applicable actions: " << applicable_actions.size()
	      << std::endl
	      << "Useful actions: " << useful_actions.size() << std::endl;
    if (verbosity > 1) {
      std::cerr << "Good actions: " << good_actions.size() << std::endl;
    }
  }


  /*
   * Delete all actions that are not useful.
   */
  for (GroundActionList::const_iterator ai = actions.begin();
       ai != actions.end(); ai++) {
    if (useful_actions.find(*ai) == useful_actions.end()) {
      delete *ai;
    }
  }

  if (verbosity > 2) {
    /*
     * Print good actions.
     */
    for (GroundActionSet::const_iterator ai = good_actions.begin();
	 ai != good_actions.end(); ai++) {
      std::cerr << "  ";
      (*ai)->print(std::cerr, problem.terms(), 0, Bindings::EMPTY);
      std::cerr << std::endl;
    }
    /*
     * Print literal values.
     */
    std::cerr << "Achievable literals:" << std::endl;
    for (AtomValueMap::const_iterator vi = atom_values_.begin();
	 vi != atom_values_.end(); vi++) {
      std::cerr << "  ";
      (*vi).first->print(std::cerr, problem.domain().predicates(),
			 problem.terms(), 0, Bindings::EMPTY);
      std::cerr << " -- " << (*vi).second << std::endl;
    }
    for (AtomValueMap::const_iterator vi = negation_values_.begin();
	 vi != negation_values_.end(); vi++) {
      std::cerr << "  (not ";
      (*vi).first->print(std::cerr, problem.domain().predicates(),
			 problem.terms(), 0, Bindings::EMPTY);
      std::cerr << ") -- " << (*vi).second << std::endl;
    }
  }
}


/* Deletes this planning graph. */
PlanningGraph::~PlanningGraph() {
  for (ActionDomainMap::const_iterator di = action_domains_.begin();
       di != action_domains_.end(); di++) {
    ActionDomain::unregister_use((*di).second);
  }
  GroundActionSet useful_actions;
  for (LiteralAchieverMap::const_iterator lai = achievers_.begin();
       lai != achievers_.end(); lai++) {
    for (ActionEffectMap::const_iterator aei = (*lai).second.begin();
	 aei != (*lai).second.end(); aei++) {
      if ((*aei).first != &problem().init_action()) {
	useful_actions.insert(dynamic_cast<const GroundAction*>((*aei).first));
      }
    }
  }
  for (GroundActionSet::const_iterator ai = useful_actions.begin();
       ai != useful_actions.end(); ai++) {
    delete *ai;
  }
}


/* Returns the heuristic value of a ground atom. */
HeuristicValue PlanningGraph::heuristic_value(const Atom& atom, size_t step_id,
					      const Bindings* bindings) const {
  if (bindings == NULL) {
    /* Assume ground atom. */
    AtomValueMap::const_iterator vi = atom_values_.find(&atom);
    return ((vi != atom_values_.end())
	    ? (*vi).second : HeuristicValue::INFINITE);
  } else {
    /* Take minimum value of ground atoms that unify. */
    HeuristicValue value = HeuristicValue::INFINITE;
    std::pair<PredicateAtomsMap::const_iterator,
      PredicateAtomsMap::const_iterator> bounds =
      predicate_atoms_.equal_range(atom.predicate());
    for (PredicateAtomsMap::const_iterator gi = bounds.first;
	 gi != bounds.second; gi++) {
      const Atom& a = *(*gi).second;
      if (bindings->unify(atom, step_id, a, 0, &problem())) {
	HeuristicValue v = heuristic_value(a, 0);
	value = min(value, v);
	if (value.zero()) {
	  return value;
	}
      }
    }
    return value;
  }
}


/* Returns the heuristic value of a negated atom. */
HeuristicValue PlanningGraph::heuristic_value(const Negation& negation,
					      size_t step_id,
					      const Bindings* bindings) const {
  if (bindings == NULL) {
    /* Assume ground negated atom. */
    AtomValueMap::const_iterator vi = negation_values_.find(&negation.atom());
    if (vi != negation_values_.end()) {
      return (*vi).second;
    } else {
      vi = atom_values_.find(&negation.atom());
      return ((vi == atom_values_.end() || !(*vi).second.zero())
	      ? HeuristicValue::ZERO_COST_UNIT_WORK
	      : HeuristicValue::INFINITE);
    }
  } else {
    /* Take minimum value of ground negated atoms that unify. */
    const Atom& atom = negation.atom();
    if (!heuristic_value(atom, step_id, bindings).zero()) {
      return HeuristicValue::ZERO;
    }
    HeuristicValue value = HeuristicValue::INFINITE;
    std::pair<PredicateAtomsMap::const_iterator,
      PredicateAtomsMap::const_iterator> bounds =
      predicate_negations_.equal_range(negation.predicate());
    for (PredicateAtomsMap::const_iterator gi = bounds.first;
	 gi != bounds.second; gi++) {
      const Atom& a = *(*gi).second;
      if (bindings->unify(atom, step_id, a, 0, &problem())) {
	HeuristicValue v = heuristic_value(a, 0);
	value = min(value, v);
	if (value.zero()) {
	  return value;
	}
      }
    }
    return value;
  }
}


/* Returns a set of achievers for the given literal. */
const ActionEffectMap*
PlanningGraph::literal_achievers(const Literal& literal) const {
  LiteralAchieverMap::const_iterator lai = achievers_.find(&literal);
  return (lai != achievers_.end()) ? & (*lai).second : NULL;
}


/* Returns the parameter domain for the given action, or NULL if the
   parameter domain is empty. */
const ActionDomain*
PlanningGraph::action_domain(const std::string& name) const {
  ActionDomainMap::const_iterator di = action_domains_.find(name);
  return (di != action_domains_.end()) ? (*di).second : NULL;
}


/* Finds an element in a LiteralActionsMap. */
bool PlanningGraph::find(const PlanningGraph::LiteralAchieverMap& m,
			 const Literal &l, const Action& a,
			 const Effect& e) const {
  LiteralAchieverMap::const_iterator lai = m.find(&l);
  if (lai != m.end()) {
    std::pair<ActionEffectMap::const_iterator,
      ActionEffectMap::const_iterator> bounds = (*lai).second.equal_range(&a);
    for (ActionEffectMap::const_iterator i = bounds.first;
	 i != bounds.second; i++) {
      if ((*i).second == &e) {
	return true;
      }
    }
  }
  return false;
}


/* ====================================================================== */
/* InvalidHeuristic */

/* Constructs an invalid heuristic exception. */
InvalidHeuristic::InvalidHeuristic(const std::string& name)
  : Exception("invalid heuristic `" + name + "'") {}


/* ====================================================================== */
/* Heuristic */

/* Constructs a heuristic from a name. */
Heuristic::Heuristic(const std::string& name) {
  *this = name;
}


/* Selects a heuristic from a name. */
Heuristic& Heuristic::operator=(const std::string& name) {
  h_.clear();
  needs_pg_ = false;
  size_t pos = 0;
  while (pos < name.length()) {
    size_t next_pos = name.find('/', pos);
    std::string key = name.substr(pos, next_pos - pos);
    const char* n = key.c_str();
    if (strcasecmp(n, "LIFO") == 0) {
      h_.push_back(LIFO);
    } else if (strcasecmp(n, "FIFO") == 0) {
      h_.push_back(FIFO);
    } else if (strcasecmp(n, "OC") == 0) {
      h_.push_back(OC);
    } else if (strcasecmp(n, "UC") == 0) {
      h_.push_back(UC);
    } else if (strcasecmp(n, "BUC") == 0) {
      h_.push_back(BUC);
    } else if (strcasecmp(n, "S+OC") == 0) {
      h_.push_back(S_PLUS_OC);
    } else if (strcasecmp(n, "UCPOP") == 0) {
      h_.push_back(UCPOP);
    } else if (strcasecmp(n, "ADD") == 0) {
      h_.push_back(ADD);
      needs_pg_ = true;
    } else if (strcasecmp(n, "ADD_COST") == 0) {
      h_.push_back(ADD_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "ADD_WORK") == 0) {
      h_.push_back(ADD_WORK);
      needs_pg_ = true;
    } else if (strcasecmp(n, "ADDR") == 0) {
      h_.push_back(ADDR);
      needs_pg_ = true;
    } else if (strcasecmp(n, "ADDR_COST") == 0) {
      h_.push_back(ADDR_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "ADDR_WORK") == 0) {
      h_.push_back(ADDR_WORK);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAKESPAN") == 0) {
      h_.push_back(MAKESPAN);
      needs_pg_ = true;
    } else {
      throw InvalidHeuristic(name);
    }
    pos = next_pos;
    if (name[pos] == '/') {
      pos++;
      if (pos >= name.length()) {
	throw InvalidHeuristic(name);
      }
    }
  }
  return *this;
}


/* Checks if this heuristic needs a planning graph. */
bool Heuristic::needs_planning_graph() const {
  return needs_pg_;
}


/* Fills the provided vector with the ranks for the given plan. */
void Heuristic::plan_rank(std::vector<float>& rank, const Plan& plan,
			  float weight, const Domain& domain,
			  const PlanningGraph* planning_graph) const {
  bool add_done = false;
  int add_cost = 0;
  int add_work = 0;
  bool addr_done = false;
  int addr_cost = 0;
  int addr_work = 0;
  for (std::vector<HVal>::const_iterator hi = h_.begin();
       hi != h_.end(); hi++) {
    HVal h = *hi;
    switch (h) {
    case LIFO:
      rank.push_back(-1.0*plan.serial_no());
      break;
    case FIFO:
      rank.push_back(plan.serial_no());
      break;
    case OC:
      rank.push_back(plan.num_open_conds());
      break;
    case UC:
      rank.push_back(plan.num_unsafes());
      break;
    case BUC:
      rank.push_back((plan.num_unsafes() > 0) ? 1 : 0);
      break;
    case S_PLUS_OC:
      rank.push_back(plan.num_steps() + weight*plan.num_open_conds());
      break;
    case UCPOP:
      rank.push_back(plan.num_steps()
		     + weight*(plan.num_open_conds() + plan.num_unsafes()));
      break;
    case ADD:
    case ADD_COST:
    case ADD_WORK:
      if (!add_done) {
	add_done = true;
	for (const Chain<OpenCondition>* occ = plan.open_conds();
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.when(),
			  open_cond.step_id(), plan, domain, *planning_graph);
	  add_cost = sum(add_cost, v.add_cost());
	  add_work = sum(add_work, v.add_work());
	}
      }
      if (h == ADD) {
	if (add_cost < INT_MAX) {
	  rank.push_back(plan.num_steps() + weight*add_cost);
	} else {
	  rank.push_back(INFINITY);
	}
      } else if (h == ADD_COST) {
	if (add_cost < INT_MAX) {
	  rank.push_back(add_cost);
	} else {
	  rank.push_back(INFINITY);
	}
      } else {
	if (add_work < INT_MAX) {
	  rank.push_back(add_work);
	} else {
	  rank.push_back(INFINITY);
	}
      }
      break;
    case ADDR:
    case ADDR_COST:
    case ADDR_WORK:
      if (!addr_done) {
	addr_done = true;
	for (const Chain<OpenCondition>* occ = plan.open_conds();
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.when(),
			  open_cond.step_id(), plan,
			  domain, *planning_graph, true);
	  addr_cost = sum(addr_cost, v.add_cost());
	  addr_work = sum(addr_work, v.add_work());
	}
      }
      if (h == ADDR) {
	if (addr_cost < INT_MAX) {
	  rank.push_back(plan.num_steps() + weight*addr_cost);
	} else {
	  rank.push_back(INFINITY);
	}
      } else if (h == ADDR_COST) {
	if (addr_cost < INT_MAX) {
	  rank.push_back(addr_cost);
	} else {
	  rank.push_back(INFINITY);
	}
      } else {
	if (addr_work < INT_MAX) {
	  rank.push_back(addr_work);
	} else {
	  rank.push_back(INFINITY);
	}
      }
      break;
    case MAKESPAN:
      std::map<std::pair<size_t, StepTime>, float> min_times;
      for (const Chain<OpenCondition>* occ = plan.open_conds();
	   occ != NULL; occ = occ->tail) {
	const OpenCondition& open_cond = occ->head;
	HeuristicValue v = formula_value(open_cond.condition(),
					 open_cond.when(),
					 open_cond.step_id(), plan, domain,
					 *planning_graph);
	StepTime st = start_time(open_cond.when());
	std::map<std::pair<size_t, StepTime>, float>::iterator di =
	  min_times.find(std::make_pair(open_cond.step_id(), st));
	if (di != min_times.end()) {
	  if (weight*v.makespan() > (*di).second) {
	    (*di).second = weight*v.makespan();
	  }
	} else {
	  min_times.insert(std::make_pair(std::make_pair(open_cond.step_id(),
							 st),
					  weight*v.makespan()));
	}
      }
      rank.push_back(plan.orderings().makespan(min_times));
      break;
    }
  }
}


/* ====================================================================== */
/* InvalidFlawSelectionOrder */

/* Constructs an invalid flaw selection order exception. */
InvalidFlawSelectionOrder::InvalidFlawSelectionOrder(const std::string& name)
  : Exception("invalid flaw selection order `" + name + "'") {}


/* ====================================================================== */
/* SelectionCriterion */

/* Output operator for selection criterion. */
std::ostream& operator<<(std::ostream& os, const SelectionCriterion& c) {
  os << '{';
  bool first = true;
  if (c.non_separable) {
    if (!first) {
      os << ',';
    }
    os << 'n';
    first = false;
  }
  if (c.separable) {
    if (!first) {
      os << ',';
    }
    os << 's';
    first = false;
  }
  if (c.open_cond) {
    if (!first) {
      os << ',';
    }
    os << 'o';
    first = false;
  }
  if (c.local_open_cond) {
    if (!first) {
      os << ',';
    }
    os << 'l';
    first = false;
  }
  if (c.static_open_cond) {
    if (!first) {
      os << ',';
    }
    os << 't';
    first = false;
  }
  if (c.unsafe_open_cond) {
    if (!first) {
      os << ',';
    }
    os << 'u';
    first = false;
  }
  os << '}';
  if (c.max_refinements < INT_MAX) {
    os << c.max_refinements;
  }
  switch (c.order) {
  case SelectionCriterion::LIFO:
    os << "LIFO";
    break;
  case SelectionCriterion::FIFO:
    os << "FIFO";
    break;
  case SelectionCriterion::RANDOM:
    os << "R";
    break;
  case SelectionCriterion::LR:
    os << "LR";
    break;
  case SelectionCriterion::MR:
    os << "MR";
    break;
  case SelectionCriterion::NEW:
    os << "NEW";
    break;
  case SelectionCriterion::REUSE:
    os << "REUSE";
    break;
  case SelectionCriterion::LC:
    os << "LC_";
    switch (c.heuristic) {
    case SelectionCriterion::ADD:
      os << "ADD";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAKESPAN:
      os << "MAKESPAN";
      break;
    }
    break;
  case SelectionCriterion::MC:
    os << "MC_";
    switch (c.heuristic) {
    case SelectionCriterion::ADD:
      os << "ADD";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAKESPAN:
      os << "MAKESPAN";
      break;
    }
    break;
  case SelectionCriterion::LW:
    os << "LW_";
    switch (c.heuristic) {
    case SelectionCriterion::ADD:
      os << "ADD";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAKESPAN:
      os << "MAKESPAN";
      break;
    }
    break;
  case SelectionCriterion::MW:
    os << "MW_";
    switch (c.heuristic) {
    case SelectionCriterion::ADD:
      os << "ADD";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAKESPAN:
      os << "MAKESPAN";
      break;
    }
    break;
  }
  return os;
}


/* ====================================================================== */
/* FlawSelectionOrder */

/* Constructs a default flaw selection order. */
FlawSelectionOrder::FlawSelectionOrder(const std::string& name) {
  *this = name;
}


/* Selects a flaw selection order from a name. */
FlawSelectionOrder& FlawSelectionOrder::operator=(const std::string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "UCPOP") == 0) {
    return *this = "{n,s}LIFO/{o}LIFO";
  } else if (strcasecmp(n, "UCPOP-LC") == 0) {
    return *this = "{n,s}LIFO/{o}LR";
  } else if (strncasecmp(n, "DSep-", 5) == 0) {
    if (strcasecmp(n + 5, "LIFO") == 0) {
      return *this = "{n}LIFO/{o}LIFO/{s}LIFO";
    } else if (strcasecmp(n + 5, "FIFO") == 0) {
      return *this = "{n}LIFO/{o}FIFO/{s}LIFO";
    } else if (strcasecmp(n + 5, "LC") == 0) {
      return *this = "{n}LIFO/{o}LR/{s}LIFO";
    }
  } else if (strncasecmp(n, "DUnf-", 5) == 0) {
    if (strcasecmp(n + 5, "LIFO") == 0) {
      return *this = "{n,s}0LIFO/{n,s}1LIFO/{o}LIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "FIFO") == 0) {
      return *this = "{n,s}0LIFO/{n,s}1LIFO/{o}FIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "LC") == 0) {
      return *this = "{n,s}0LIFO/{n,s}1LIFO/{o}LR/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "Gen") == 0) {
      return *this = "{n,s,o}0LIFO/{n,s,o}1LIFO/{n,s,o}LIFO";
    }
  } else if (strncasecmp(n, "DRes-", 5) == 0) {
    if (strcasecmp(n + 5, "LIFO") == 0) {
      return *this = "{n,s}0LIFO/{o}LIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "FIFO") == 0) {
      return *this = "{n,s}0LIFO/{o}FIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "LC") == 0) {
      return *this = "{n,s}0LIFO/{o}LR/{n,s}LIFO";
    }
  } else if (strncasecmp(n, "DEnd-", 5) == 0) {
    if (strcasecmp(n + 5, "LIFO") == 0) {
      return *this = "{o}LIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "FIFO") == 0) {
      return *this = "{o}FIFO/{n,s}LIFO";
    } else if (strcasecmp(n + 5, "LC") == 0) {
      return *this = "{o}LR/{n,s}LIFO";
    }
  } else if (strcasecmp(n, "LCFR") == 0) {
    return *this = "{n,s,o}LR";
  } else if (strcasecmp(n, "LCFR-DSep") == 0) {
    return *this = "{n,o}LR/{s}LR";
  } else if (strcasecmp(n, "ZLIFO") == 0) {
    return *this = "{n}LIFO/{o}0LIFO/{o}1NEW/{o}LIFO/{s}LIFO";
  } else if (strcasecmp(n, "ZLIFO*") == 0) {
    return *this = "{o}0LIFO/{n,s}LIFO/{o}1NEW/{o}LIFO";
  } else if (strcasecmp(n, "Static") == 0) {
    return *this = "{t}LIFO/{n,s}LIFO/{o}LIFO";
  } else if (strcasecmp(n, "LCFR-Loc") == 0) {
    return *this = "{n,s,l}LR";
  } else if (strcasecmp(n, "LCFR-Conf") == 0) {
    return *this = "{n,s,u}LR/{o}LR";
  } else if (strcasecmp(n, "LCFR-Loc-Conf") == 0) {
    return *this = "{n,s,u}LR/{l}LR";
  } else if (strcasecmp(n, "MC") == 0) {
    return *this = "{n,s}LR/{o}MC_add";
  } else if (strcasecmp(n, "MC-Loc") == 0) {
    return *this = "{n,s}LR/{l}MC_add";
  } else if (strcasecmp(n, "MC-Loc-Conf") == 0) {
    return *this = "{n,s}LR/[u}MC_add/{l}MC_add";
  } else if (strcasecmp(n, "MW") == 0) {
    return *this = "{n,s}LR/{o}MW_add";
  } else if (strcasecmp(n, "MW-Loc") == 0) {
    return *this = "{n,s}LR/{l}MW_add";
  } else if (strcasecmp(n, "MW-Loc-Conf") == 0) {
    return *this = "{n,s}LR/{u}MW_add/{l}MW_add";
  }
  selection_criteria_.clear();
  needs_pg_ = false;
  first_unsafe_criterion_ = INT_MAX;
  last_unsafe_criterion_ = 0;
  first_open_cond_criterion_ = INT_MAX;
  last_open_cond_criterion_ = 0;
  int non_separable_max_refinements = -1;
  int separable_max_refinements = -1;
  int open_cond_max_refinements = -1;
  size_t pos = 0;
  while (pos < name.length()) {
    if (name[pos] != '{') {
      throw InvalidFlawSelectionOrder(name);
    }
    pos++;
    SelectionCriterion criterion;
    criterion.non_separable = false;
    criterion.separable = false;
    criterion.open_cond = false;
    criterion.local_open_cond = false;
    criterion.static_open_cond = false;
    criterion.unsafe_open_cond = false;
    do {
      switch (name[pos]) {
      case 'n':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  criterion.non_separable = true;
	  if (first_unsafe_criterion_ > last_unsafe_criterion_) {
	    first_unsafe_criterion_ = selection_criteria_.size();
	  }
	  last_unsafe_criterion_ = selection_criteria_.size();
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      case 's':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  criterion.separable = true;
	  if (first_unsafe_criterion_ > last_unsafe_criterion_) {
	    first_unsafe_criterion_ = selection_criteria_.size();
	  }
	  last_unsafe_criterion_ = selection_criteria_.size();
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      case 'o':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  criterion.open_cond = true;
	  criterion.local_open_cond = false;
	  criterion.static_open_cond = false;
	  criterion.unsafe_open_cond = false;
	  if (first_open_cond_criterion_ > last_open_cond_criterion_) {
	    first_open_cond_criterion_ = selection_criteria_.size();
	  }
	  last_open_cond_criterion_ = selection_criteria_.size();
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      case 'l':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  if (!criterion.open_cond) {
	    criterion.local_open_cond = true;
	    if (first_open_cond_criterion_ > last_open_cond_criterion_) {
	      first_open_cond_criterion_ = selection_criteria_.size();
	    }
	    last_open_cond_criterion_ = selection_criteria_.size();
	  }
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      case 't':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  if (!criterion.open_cond) {
	    criterion.static_open_cond = true;
	    if (first_open_cond_criterion_ > last_open_cond_criterion_) {
	      first_open_cond_criterion_ = selection_criteria_.size();
	    }
	    last_open_cond_criterion_ = selection_criteria_.size();
	  }
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      case 'u':
	pos++;
	if (name[pos] == ',' || name[pos] == '}') {
	  if (!criterion.open_cond) {
	    criterion.unsafe_open_cond = true;
	    if (first_open_cond_criterion_ > last_open_cond_criterion_) {
	      first_open_cond_criterion_ = selection_criteria_.size();
	    }
	    last_open_cond_criterion_ = selection_criteria_.size();
	  }
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
	break;
      default:
	throw InvalidFlawSelectionOrder(name);
      }
      if (name[pos] == ',') {
	pos++;
	if (name[pos] == '}') {
	  throw InvalidFlawSelectionOrder(name);
	}
      }
    } while (name[pos] != '}');
    pos++;
    size_t next_pos = pos;
    while (name[next_pos] >= '0' && name[next_pos] <= '9') {
      next_pos++;
    }
    if (next_pos > pos) {
      std::string number = name.substr(pos, next_pos - pos);
      criterion.max_refinements = atoi(number.c_str());
      pos = next_pos;
    } else {
      criterion.max_refinements = INT_MAX;
    }
    next_pos = name.find('/', pos);
    std::string key = name.substr(pos, next_pos - pos);
    n = key.c_str();
    if (strcasecmp(n, "LIFO") == 0) {
      criterion.order = SelectionCriterion::LIFO;
    } else if (strcasecmp(n, "FIFO") == 0) {
      criterion.order = SelectionCriterion::FIFO;
    } else if (strcasecmp(n, "R") == 0) {
      criterion.order = SelectionCriterion::RANDOM;
    } else if (strcasecmp(n, "LR") == 0) {
      criterion.order = SelectionCriterion::LR;
    } else if (strcasecmp(n, "MR") == 0) {
      criterion.order = SelectionCriterion::MR;
    } else {
      if (criterion.non_separable || criterion.separable) {
	/* No other orders that the above can be used with threats. */
	throw InvalidFlawSelectionOrder(name);
      }
      if (strcasecmp(n, "NEW") == 0) {
	criterion.order = SelectionCriterion::NEW;
      } else if (strcasecmp(n, "REUSE") == 0) {
	criterion.order = SelectionCriterion::REUSE;
      } else if (strncasecmp(n, "LC_", 3) == 0) {
	criterion.order = SelectionCriterion::LC;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "ADD") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "ADDR") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAKESPAN") == 0) {
	  criterion.heuristic = SelectionCriterion::MAKESPAN;
	  criterion.reuse = false;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "MC_", 3) == 0) {
	criterion.order = SelectionCriterion::MC;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "ADD") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "ADDR") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAKESPAN") == 0) {
	  criterion.heuristic = SelectionCriterion::MAKESPAN;
	  criterion.reuse = false;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "LW_", 3) == 0) {
	criterion.order = SelectionCriterion::LW;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "ADD") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "ADDR") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "MW_", 3) == 0) {
	criterion.order = SelectionCriterion::MW;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "ADD") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "ADDR") == 0) {
	  criterion.heuristic = SelectionCriterion::ADD;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else {
	throw InvalidFlawSelectionOrder(name);
      }
    }
    if (criterion.non_separable) {
      non_separable_max_refinements = std::max(criterion.max_refinements,
					       non_separable_max_refinements);
    }
    if (criterion.separable) {
      separable_max_refinements = std::max(criterion.max_refinements,
					   separable_max_refinements);
    }
    if (criterion.open_cond || criterion.local_open_cond) {
      open_cond_max_refinements = std::max(criterion.max_refinements,
					   open_cond_max_refinements);
    }
    selection_criteria_.push_back(criterion);
    pos = next_pos;
    if (name[pos] == '/') {
      pos++;
      if (pos >= name.length()) {
	throw InvalidFlawSelectionOrder(name);
      }
    }
  }
  if (non_separable_max_refinements < INT_MAX
      || separable_max_refinements < INT_MAX
      || open_cond_max_refinements < INT_MAX) {
    /* Incomplete flaw selection order. */
    throw InvalidFlawSelectionOrder(name);
  }
  return *this;
}


/* Checks if this flaw order needs a planning graph. */
bool FlawSelectionOrder::needs_planning_graph() const {
  return needs_pg_;
}


/* Seaches threats for a flaw to select. */
int FlawSelectionOrder::select_unsafe(FlawSelection& selection,
				      const Plan& plan, const Problem& problem,
				      int first_criterion,
				      int last_criterion) const {
  if (first_criterion > last_criterion || plan.unsafes() == NULL) {
    return INT_MAX;
  }
  /* Loop through usafes. */
  for (const Chain<Unsafe>* uc = plan.unsafes();
       uc != NULL && first_criterion <= last_criterion; uc = uc->tail) {
    const Unsafe& unsafe = uc->head;
    if (verbosity > 1) {
      std::cerr << "(considering ";
      unsafe.print(std::cerr, problem.domain().predicates(),
		   problem.terms(), Bindings::EMPTY);
      std::cerr << ")" << std::endl;
    }
    int refinements = -1;
    int separable = -1;
    int promotable = -1;
    int demotable = -1;
    /* Loop through selection criteria that are within limits. */
    for (int c = first_criterion; c <= last_criterion; c++) {
      const SelectionCriterion& criterion = selection_criteria_[c];
      /* If criterion applies only to one type of threats, make sure
         we know which type of threat this is. */
      if (criterion.non_separable != criterion.separable && separable < 0) {
	separable = plan.separable(unsafe);
	if (separable < 0) {
	  refinements = separable = 0;
	}
      }
      /* Test if criterion applies. */
      if ((criterion.non_separable && criterion.separable)
	  || (criterion.separable && separable > 0)
	  || (criterion.non_separable && separable == 0)) {
	/* Right type of threat, so now check if the refinement
           constraint is satisfied. */
	if (criterion.max_refinements >= 3
	    || plan.unsafe_refinements(refinements, separable, promotable,
				       demotable, unsafe,
				       criterion.max_refinements)) {
	  /* Refinement constraint is satisfied, so criterion applies. */
	  switch (criterion.order) {
	  case SelectionCriterion::LIFO:
	    selection.flaw = &unsafe;
	    selection.criterion = c;
	    last_criterion = c - 1;
	    if (verbosity > 1) {
	      std::cerr << "selecting ";
	      unsafe.print(std::cerr, problem.domain().predicates(),
			   problem.terms(), Bindings::EMPTY);
	      std::cerr << " by criterion " << criterion << std::endl;
	    }
	    break;
	  case SelectionCriterion::FIFO:
	    selection.flaw = &unsafe;
	    selection.criterion = c;
	    last_criterion = c;
	    if (verbosity > 1) {
	      std::cerr << "selecting ";
	      unsafe.print(std::cerr, problem.domain().predicates(),
			   problem.terms(), Bindings::EMPTY);
	      std::cerr << " by criterion " << criterion << std::endl;
	    }
	    break;
	  case SelectionCriterion::RANDOM:
	    if (c == selection.criterion) {
	      selection.streak++;
	    } else {
	      selection.streak = 1;
	    }
	    if (rand01ex() < 1.0/selection.streak) {
	      selection.flaw = &unsafe;
	      selection.criterion = c;
	      last_criterion = c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		unsafe.print(std::cerr, problem.domain().predicates(),
			     problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion << std::endl;
	      }
	    }
	    break;
	  case SelectionCriterion::LR:
	    if (c < selection.criterion
		|| plan.unsafe_refinements(refinements, separable, promotable,
					   demotable, unsafe,
					   selection.rank - 1)) {
	      selection.flaw = &unsafe;
	      selection.criterion = c;
	      plan.unsafe_refinements(refinements, separable, promotable,
				      demotable, unsafe, INT_MAX);
	      selection.rank = refinements;
	      last_criterion = (refinements == 0) ? c - 1 : c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		unsafe.print(std::cerr, problem.domain().predicates(),
			     problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion
			  << " with rank " << refinements << std::endl;
	      }
	    }
	    break;
	  case SelectionCriterion::MR:
	    plan.unsafe_refinements(refinements, separable, promotable,
				    demotable, unsafe, INT_MAX);
	    if (c < selection.criterion
		|| refinements > selection.rank) {
	      selection.flaw = &unsafe;
	      selection.criterion = c;
	      selection.rank = refinements;
	      last_criterion = (refinements == 3) ? c - 1 : c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		unsafe.print(std::cerr, problem.domain().predicates(),
			     problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion
			  << " with rank " << refinements << std::endl;
	      }
	    }
	    break;
	  default:
	    /* No other ordering criteria apply to threats. */
	    break;
	  }
	}
      }
    }
  }
  return last_criterion;
}


/* Seaches open conditions for a flaw to select. */
int FlawSelectionOrder::select_open_cond(FlawSelection& selection,
					 const Plan& plan,
					 const Problem& problem,
					 const PlanningGraph* pg,
					 int first_criterion,
					 int last_criterion) const {
  if (first_criterion > last_criterion || plan.open_conds() == NULL) {
    return INT_MAX;
  }
  size_t local_id = 0;
  /* Loop through open conditions. */
  for (const Chain<OpenCondition>* occ = plan.open_conds();
       occ != NULL && first_criterion <= last_criterion; occ = occ->tail) {
    const OpenCondition& open_cond = occ->head;
    if (verbosity > 1) {
      std::cerr << "(considering ";
      open_cond.print(std::cerr, problem.domain().predicates(),
		      problem.terms(), Bindings::EMPTY);
      std::cerr << ")" << std::endl;
    }
    if (local_id == 0) {
      local_id = open_cond.step_id();
    }
    bool local = (open_cond.step_id() == local_id);
    int is_static = -1;
    int is_unsafe = -1;
    int refinements = -1;
    int addable = -1;
    int reusable = -1;
    /* Loop through selection criteria that are within limits. */
    for (int c = first_criterion; c <= last_criterion; c++) {
      const SelectionCriterion& criterion = selection_criteria_[c];
      if (criterion.local_open_cond && !local
	  && !criterion.static_open_cond && !criterion.unsafe_open_cond) {
	if (c == last_criterion) {
	  last_criterion--;
	}
	continue;
      }
      /* If criterion applies only to one type of open condition, make
         sure we know which type of open condition this is. */
      if (criterion.static_open_cond && is_static < 0) {
	is_static = open_cond.is_static(problem.domain()) ? 1 : 0;
      }
      if (criterion.unsafe_open_cond && is_unsafe < 0) {
	is_unsafe = (plan.unsafe_open_condition(open_cond)) ? 1 : 0;
      }
      /* Test if criterion applies. */
      if (criterion.open_cond
	  || (criterion.local_open_cond && local)
	  || (criterion.static_open_cond && is_static > 0)
	  || (criterion.unsafe_open_cond && is_unsafe > 0)) {
	/* Right type of open condition, so now check if the
           refinement constraint is satisfied. */
	if (criterion.max_refinements == INT_MAX
	    || plan.open_cond_refinements(refinements, addable, reusable,
					  open_cond,
					  criterion.max_refinements)) {
	  /* Refinement constraint is satisfied, so criterion applies. */
	  switch (criterion.order) {
	  case SelectionCriterion::LIFO:
	    selection.flaw = &open_cond;
	    selection.criterion = c;
	    last_criterion = c - 1;
	    if (verbosity > 1) {
	      std::cerr << "selecting ";
	      open_cond.print(std::cerr, problem.domain().predicates(),
			      problem.terms(), Bindings::EMPTY);
	      std::cerr << " by criterion " << criterion << std::endl;
	    }
	    break;
	  case SelectionCriterion::FIFO:
	    selection.flaw = &open_cond;
	    selection.criterion = c;
	    last_criterion = c;
	    if (verbosity > 1) {
	      std::cerr << "selecting ";
	      open_cond.print(std::cerr, problem.domain().predicates(),
			      problem.terms(), Bindings::EMPTY);
	      std::cerr << " by criterion " << criterion << std::endl;
	    }
	    break;
	  case SelectionCriterion::RANDOM:
	    if (c == selection.criterion) {
	      selection.streak++;
	    } else {
	      selection.streak = 1;
	    }
	    if (rand01ex() < 1.0/selection.streak) {
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      last_criterion = c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		open_cond.print(std::cerr, problem.domain().predicates(),
				problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion << std::endl;
	      }
	    }
	    break;
	  case SelectionCriterion::LR:
	    if (c < selection.criterion
		|| plan.open_cond_refinements(refinements, addable, reusable,
					      open_cond, selection.rank - 1)) {
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      plan.open_cond_refinements(refinements, addable, reusable,
					 open_cond, INT_MAX);
	      selection.rank = refinements;
	      last_criterion = (refinements == 0) ? c - 1 : c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		open_cond.print(std::cerr, problem.domain().predicates(),
				problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion
			  << " with rank " << refinements << std::endl;
	      }
	    }
	    break;
	  case SelectionCriterion::MR:
	    plan.open_cond_refinements(refinements, addable, reusable,
				       open_cond, INT_MAX);
	    if (c < selection.criterion
		|| refinements > selection.rank) {
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      selection.rank = refinements;
	      last_criterion = c;
	      if (verbosity > 1) {
		std::cerr << "selecting ";
		open_cond.print(std::cerr, problem.domain().predicates(),
				problem.terms(), Bindings::EMPTY);
		std::cerr << " by criterion " << criterion
			  << " with rank " << refinements << std::endl;
	      }
	    }
	    break;
	  case SelectionCriterion::NEW:
	    {
	      bool has_new = false;
	      if (addable < 0) {
		const Literal* literal = open_cond.literal();
		if (literal != NULL) {
		  has_new = !plan.addable_steps(addable, *literal,
						open_cond, 0);
		}
	      } else {
		has_new = (addable > 0);
	      }
	      if (has_new || c < selection.criterion) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		last_criterion = has_new ? c - 1 : c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion;
		  if (has_new) {
		    std::cerr << " with new";
		  }
		  std::cerr << std::endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::REUSE:
	    {
	      bool has_reuse = false;
	      if (reusable < 0) {
		const Literal* literal = open_cond.literal();
		if (literal != NULL) {
		  has_reuse = !plan.reusable_steps(reusable, *literal,
						   open_cond, 0);
		}
	      } else {
		has_reuse = (reusable > 0);
	      }
	      if (has_reuse || c < selection.criterion) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		last_criterion = has_reuse ? c - 1 : c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion;
		  if (has_reuse) {
		    std::cerr << " with reuse";
		  }
		  std::cerr << std::endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::LC:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.when(),
			      open_cond.step_id(), plan,
			      problem.domain(), *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::ADD)
			  ? h.add_cost() : int(h.makespan() + 0.5));
	      if (c < selection.criterion || rank < selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = (rank == 0) ? c - 1 : c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion
			    << " with rank " << rank << std::endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::MC:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.when(),
			      open_cond.step_id(), plan,
			      problem.domain(), *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::ADD)
			  ? h.add_cost() : int(h.makespan() + 0.5));
	      if (c < selection.criterion || rank > selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion
			    << " with rank " << rank << std::endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::LW:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.when(),
			      open_cond.step_id(), plan,
			      problem.domain(), *pg, criterion.reuse);
	      int rank = h.add_work();
	      if (c < selection.criterion || rank < selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = (rank == 0) ? c - 1 : c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion
			    << " with rank " << rank << std::endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::MW:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.when(),
			      open_cond.step_id(), plan,
			      problem.domain(), *pg, criterion.reuse);
	      int rank = h.add_work();
	      if (c < selection.criterion || rank > selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = c;
		if (verbosity > 1) {
		  std::cerr << "selecting ";
		  open_cond.print(std::cerr, problem.domain().predicates(),
				  problem.terms(), Bindings::EMPTY);
		  std::cerr << " by criterion " << criterion
			    << " with rank " << rank << std::endl;
		}
	      }
	    }
	    break;
	  }
	}
      }
    }
  }
  return last_criterion;
}


/* Selects a flaw from the flaws of the given plan. */
const Flaw& FlawSelectionOrder::select(const Plan& plan,
				       const Problem& problem,
				       const PlanningGraph* pg) const {
  FlawSelection selection;
  selection.criterion = INT_MAX;
  int last_criterion = select_unsafe(selection, plan, problem,
				     first_unsafe_criterion_,
				     last_unsafe_criterion_);
  select_open_cond(selection, plan, problem, pg, first_open_cond_criterion_,
		   std::min(last_open_cond_criterion_, last_criterion));
  return *selection.flaw;
}
