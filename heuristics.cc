/*
 * $Id: heuristics.cc,v 1.16 2002-01-02 19:28:25 lorens Exp $
 */
#include <set>
#include <typeinfo>
#include <cmath>
#include "heuristics.h"
#include "plans.h"
#include "bindings.h"
#include "orderings.h"
#include "flaws.h"
#include "problems.h"
#include "domains.h"
#include "debug.h"


/* ====================================================================== */
/* HeuristicValue */

/* Returns the sum of two integers, avoiding overflow. */
static int sum(int n, int m) {
  return (INT_MAX - n > m) ? n + m : INT_MAX;
}

    
/* A zero heuristic value. */
const HeuristicValue HeuristicValue::ZERO = HeuristicValue();
/* A zero cost, unit work, heuristic value. */
const HeuristicValue
HeuristicValue::ZERO_COST_UNIT_WORK = HeuristicValue(0, 1, 0, 1);
/* An infinite heuristic value. */
const HeuristicValue
HeuristicValue::INFINITE = HeuristicValue(INT_MAX, INT_MAX, INT_MAX, INT_MAX);


/* Constructs a zero heuristic value. */
HeuristicValue::HeuristicValue()
  : max_cost_(0), max_work_(0), sum_cost_(0), sum_work_(0) {}


/* Constructs a heuristic value. */
HeuristicValue::HeuristicValue(int max_cost, int max_work,
			       int sum_cost, int sum_work)
  : max_cost_(max_cost), max_work_(max_work),
    sum_cost_(sum_cost), sum_work_(sum_work) {}


/* Returns the cost according to the MAX heuristic. */
int HeuristicValue::max_cost() const {
  return max_cost_;
}


/* Returns the work according to the MAX heuristic. */
int HeuristicValue::max_work() const {
  return max_work_;
}


/* Returns the cost according to the SUM heurisitc. */
int HeuristicValue::sum_cost() const {
  return sum_cost_;
}


/* Returns the work according to the SUM heuristic. */
int HeuristicValue::sum_work() const {
  return sum_work_;
}


/* Checks if this heuristic value is zero. */
bool HeuristicValue::zero() const {
  return max_cost_ == 0;
}


/* Checks if this heuristic value is infinite. */
bool HeuristicValue::infinite() const {
  return max_cost_ == INT_MAX;
}


/* Adds the given heuristic value to this heuristic value. */
HeuristicValue& HeuristicValue::operator+=(const HeuristicValue& v) {
  if (max_cost_ < v.max_cost_) {
    max_cost_ = v.max_cost_;
  }
  max_work_ = sum(max_work_, v.max_work_);
  sum_cost_ = sum(sum_cost_, v.sum_cost_);
  sum_work_ = sum(sum_work_, v.sum_work_);
  return *this;
}


/* Adds the given cost to this heuristic value. */
void HeuristicValue::add_cost(int c) {
  max_cost_ = sum(max_cost_, c);
  sum_cost_ = sum(sum_cost_, c);
}


/* Adds the given work to this heuristic value. */
void HeuristicValue::add_work(int w) {
  max_work_ = sum(max_work_, w);
  sum_work_ = sum(sum_work_, w);
}


/* Prints this object on the given stream. */
void HeuristicValue::print(ostream& os) const {
  os << "MAX<" << max_cost_ << ',' << max_work_ << '>'
     << " SUM<" << sum_cost_ << ',' << sum_work_ << '>';
}


/* Equality operator for heuristic values. */
bool operator==(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1.max_cost() == v2.max_cost() && v1.max_work() == v2.max_work()
	  && v1.sum_cost() == v2.sum_cost() && v1.sum_work() == v2.sum_work());
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
  return (v1.max_cost() <= v2.max_cost() && v1.max_work() <= v2.max_work()
	  && v1.sum_cost() <= v2.sum_cost() && v1.sum_work() <= v2.sum_work());
}


/* Greater than or equal to operator for heuristic values. */
bool operator>=(const HeuristicValue& v1, const HeuristicValue& v2) {
  return (v1.max_cost() >= v2.max_cost() && v1.max_work() >= v2.max_work()
	  && v1.sum_cost() >= v2.sum_cost() && v1.sum_work() >= v2.sum_work());
}


/* Returns the componentwise minimum heuristic value, given two
   heuristic values. */
HeuristicValue min(const HeuristicValue& v1, const HeuristicValue& v2) {
  int max_cost, max_work, sum_cost, sum_work;
  if (v1.max_cost() == v2.max_cost()) {
    max_cost = v1.max_cost();
    max_work = min(v1.max_work(), v2.max_work());
  } else if (v1.max_cost() < v2.max_cost()) {
    max_cost = v1.max_cost();
    max_work = v1.max_work();
  } else {
    max_cost = v2.max_cost();
    max_work = v2.max_work();
  }
  if (v1.sum_cost() == v2.sum_cost()) {
    sum_cost = v1.sum_cost();
    sum_work = min(v1.sum_work(), v2.sum_work());
  } else if (v1.sum_cost() < v2.sum_cost()) {
    sum_cost = v1.sum_cost();
    sum_work = v1.sum_work();
  } else {
    sum_cost = v2.sum_cost();
    sum_work = v2.sum_work();
  }
  return HeuristicValue(max_cost, max_work, sum_cost, sum_work);
}


/* ====================================================================== */
/* Heuristic evaluation functions for formulas. */

/* Returns the heuristic value of this formula. */
void Constant::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b) const {
  h = HeuristicValue::ZERO;
}


/* Returns the heuristic value of this formula. */
void Atom::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			   const Bindings* b) const {
  h = pg.heuristic_value(*this, b);
}


/* Returns the heuristic value of this formula. */
void Negation::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b) const {
  h = pg.heuristic_value(*this, b);
}


/* Returns the heuristic vaue of this formula. */
void Equality::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
			       const Bindings* b) const {
  if (b == NULL) {
    h = HeuristicValue::ZERO;
  } else {
    h = (b->consistent_with(*this)
	 ? HeuristicValue::ZERO : HeuristicValue::INFINITE);
  }
}


/* Returns the heuristic value of this formula. */
void Inequality::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
				 const Bindings* b) const {
  if (b == NULL) {
    h = HeuristicValue::ZERO;
  } else {
    h = (b->consistent_with(*this)
	 ? HeuristicValue::ZERO : HeuristicValue::INFINITE);
  }
}


/* Returns the heuristic value of this formula. */
void Conjunction::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
				  const Bindings* b) const {
  h = HeuristicValue::ZERO;
  for (FormulaListIter fi = conjuncts.begin();
       fi != conjuncts.end() && !h.infinite(); fi++) {
    HeuristicValue hi;
    (*fi)->heuristic_value(hi, pg, b);
    h += hi;
  }
}


/* Returns the heuristic value of this formula. */
void Disjunction::heuristic_value(HeuristicValue& h, const PlanningGraph& pg,
				  const Bindings* b) const {
  h = HeuristicValue::INFINITE;
  for (FormulaListIter fi = disjuncts.begin();
       fi != disjuncts.end() && !h.zero(); fi++) {
    HeuristicValue hi;
    (*fi)->heuristic_value(hi, pg, b);
    h = min(h, hi);
  }
}


/* Returns the heuristic value of this formula. */
void QuantifiedFormula::heuristic_value(HeuristicValue& h,
					const PlanningGraph& pg,
					const Bindings* b) const {
  throw Unimplemented("heuristic value of quantified formula not implemented");
}


/* ====================================================================== */
/* PlanningGraph */

/* Constructs a planning graph. */
PlanningGraph::PlanningGraph(const Problem& problem, bool domain_constraints) {
  /*
   * Find all consistent action instantiations.
   */
  GroundActionList actions;
  problem.instantiated_actions(actions);
  if (verbosity > 0) {
    cout << endl << "instantiated actions: " << actions.size() << endl;
  }

  /*
   * Add initial conditions at level 0.
   */
  for (AtomListIter gi = problem.init.add_list.begin();
       gi != problem.init.add_list.end(); gi++) {
    const Atom& atom = **gi;
    if (problem.domain.static_predicate(atom.predicate())) {
      atom_values.insert(make_pair(&atom, HeuristicValue::ZERO));
    } else {
      atom_values.insert(make_pair(&atom,
				   HeuristicValue::ZERO_COST_UNIT_WORK));
    }
  }

  /*
   * Generate the rest of the levels until no change occurs.
   */
  bool changed;
  int level = 0;
  set<const GroundAction*> applicable_actions;
  do {
    if (verbosity > 3) {
      /*
       * Print literal values at this level.
       */
      cout << "Literal values at level " << level << ":" << endl;
      for (AtomValueMapIter vi = atom_values.begin();
	   vi != atom_values.end(); vi++) {
	cout << "  " << *(*vi).first << " -- " << (*vi).second << endl;
      }
      for (AtomValueMapIter vi = negation_values.begin();
	   vi != negation_values.end(); vi++) {
	cout << "  " << !*(*vi).first << " -- " << (*vi).second << endl;
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
    for (GroundActionListIter ai = actions.begin();
	 ai != actions.end(); ai++) {
      const GroundAction& action = **ai;
      HeuristicValue pre_value;
      action.precondition.heuristic_value(pre_value, *this);
      if (!pre_value.infinite()) {
	/* Precondition is achievable at this level. */
	if (applicable_actions.find(&action) == applicable_actions.end()) {
	  /* First time this action is applicable. */
	  applicable_actions.insert(&action);
	  if (domain_constraints && !action.arguments.empty()) {
	    ActionDomainMapIter di = action_domains.find(action.name);
	    if (di == action_domains.end()) {
	      ActionDomain* domain = new ActionDomain(action.arguments);
	      action_domains.insert(make_pair(action.name, domain));
	    } else {
	      (*di).second->add(action.arguments);
	    }
	  }
	}
	for (EffectListIter ei = action.effects.begin();
	     ei != action.effects.end(); ei++) {
	  const Effect& effect = **ei;
	  HeuristicValue cond_value;
	  effect.condition.heuristic_value(cond_value, *this);
	  if (!cond_value.infinite()) {
	    /* Effect condition is achievable at this level. */
	    cond_value += pre_value;
	    cond_value.add_cost(1);

	    /*
	     * Update heuristic values of atoms in add list of effect.
	     */
	    for (AtomListIter gi = effect.add_list.begin();
		 gi != effect.add_list.end(); gi++) {
	      const Atom& atom = **gi;
	      if (achieves.find(make_pair(&atom, &action)) == achieves.end()) {
		achieves.insert(make_pair(&atom, &action));
		if (verbosity > 4) {
		  cout << "  " << action.action_formula() << " achieves "
		       << atom << endl;
		}
	      }
	      AtomValueMapIter vi = new_atom_values.find(&atom);
	      if (vi == new_atom_values.end()) {
		vi = atom_values.find(&atom);
		if (vi == atom_values.end()) {
		  /* First level this atom is achieved. */
		  HeuristicValue new_value = cond_value;
		  new_value.add_work(1);
		  new_atom_values.insert(make_pair(&atom, new_value));
		  changed = true;
		  continue;
		}
	      }
	      /* This atom has been achieved earlier. */
	      HeuristicValue old_value = (*vi).second;
	      HeuristicValue new_value = cond_value;
	      new_value.add_work(1);
	      new_value = min(new_value, old_value);
	      if (new_value < old_value) {
		new_atom_values[&atom] = new_value;
		changed = true;
	      }
	    }

	    /*
	     * Update heuristic values of negated atoms in delete list
	     * of effect.
	     */
	    for (NegationListIter gi = effect.del_list.begin();
		 gi != effect.del_list.end(); gi++) {
	      const Negation& negation = **gi;
	      if (achieves.find(make_pair(&negation, &action))
		  == achieves.end()) {
		achieves.insert(make_pair(&negation, &action));
		if (verbosity > 4) {
		  cout << "  " << action.action_formula() << " achieves "
		       << negation << endl;
		}
	      }
	      AtomValueMapIter vi = new_negation_values.find(&negation.atom);
	      if (vi == new_negation_values.end()) {
		vi = negation_values.find(&negation.atom);
		if (vi == negation_values.end()) {
		  if (heuristic_value(negation.atom).zero()) {
		    /* First level this negated atom is achieved. */
		    HeuristicValue new_value = cond_value;
		    new_value.add_work(1);
		    new_negation_values.insert(make_pair(&negation.atom,
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
	      new_value.add_work(1);
	      new_value = min(new_value, old_value);
	      if (new_value < old_value) {
		new_negation_values[&negation.atom] = new_value;
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
    for (AtomValueMapIter vi = new_atom_values.begin();
	 vi != new_atom_values.end(); vi++) {
      atom_values[(*vi).first] = (*vi).second;
    }
    /*
     * Add achieved negated atoms to previously achieved negated atoms.
     */
    for (AtomValueMapIter vi = new_negation_values.begin();
	 vi != new_negation_values.end(); vi++) {
      negation_values[(*vi).first] = (*vi).second;
    }
  } while (changed);

  /*
   * Map predicates to achievable ground atoms.
   */
  for (AtomValueMapIter vi = atom_values.begin();
       vi != atom_values.end(); vi++) {
    const Atom& atom = *(*vi).first;
    predicate_atoms.insert(make_pair(atom.predicate(), &atom));
  }

  /*
   * Map predicates to achievable negated ground atoms.
   */
  for (AtomValueMapIter vi = negation_values.begin();
       vi != negation_values.end(); vi++) {
    const Atom& atom = *(*vi).first;
    predicate_negations.insert(make_pair(atom.predicate(), &atom));
  }

  if (verbosity > 0) {
    /*
     * Print statistics.
     */
    cout << "applicable actions: " << applicable_actions.size() << endl;
    if (verbosity > 2) {
      /*
       * Print applicable actions.
       */
      for (set<const GroundAction*>::const_iterator ai =
	     applicable_actions.begin();
	   ai != applicable_actions.end(); ai++) {
	cout << "  " << (*ai)->action_formula() << endl;
      }
      /*
       * Print literal values.
       */
      cout << "Literal values:" << endl;
      for (AtomValueMapIter vi = atom_values.begin();
	   vi != atom_values.end(); vi++) {
	cout << "  " << *(*vi).first << " -- " << (*vi).second << endl;
      }
      for (AtomValueMapIter vi = negation_values.begin();
	   vi != negation_values.end(); vi++) {
	cout << "  " << !*(*vi).first << " -- " << (*vi).second << endl;
      }
    }
  }
}


/* Returns the heuristic value of a ground atom. */
HeuristicValue PlanningGraph::heuristic_value(const Atom& atom,
					      const Bindings* bindings) const {
  if (bindings == NULL) {
    /* Assume ground atom. */
    AtomValueMapIter vi = atom_values.find(&atom);
    return (vi != atom_values.end()) ? (*vi).second : HeuristicValue::INFINITE;
  } else {
    /* Take minimum value of ground atoms that unify. */
    HeuristicValue value = HeuristicValue::INFINITE;
    pair<PredicateAtomsMapIter, PredicateAtomsMapIter> bounds =
      predicate_atoms.equal_range(atom.predicate());
    for (PredicateAtomsMapIter gi = bounds.first; gi != bounds.second; gi++) {
      const Atom& a = *(*gi).second;
      if (bindings->unify(atom, a)) {
	HeuristicValue v = heuristic_value(a);
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
					      const Bindings* bindings) const {
  if (bindings == NULL) {
    /* Assume ground negated atom. */
    AtomValueMapIter vi = negation_values.find(&negation.atom);
    if (vi != negation_values.end()) {
      return (*vi).second;
    } else {
      vi = atom_values.find(&negation.atom);
      return ((vi == atom_values.end() || !(*vi).second.zero())
	      ? HeuristicValue::ZERO_COST_UNIT_WORK
	      : HeuristicValue::INFINITE);
    }
  } else {
    /* Take minimum value of ground negated atoms that unify. */
    const Atom& atom = negation.atom;
    if (!heuristic_value(atom, bindings).zero()) {
      return HeuristicValue::ZERO;
    }
    HeuristicValue value = HeuristicValue::INFINITE;
    pair<PredicateAtomsMapIter, PredicateAtomsMapIter> bounds =
      predicate_negations.equal_range(negation.predicate());
    for (PredicateAtomsMapIter gi = bounds.first; gi != bounds.second; gi++) {
      const Atom& a = *(*gi).second;
      if (bindings->unify(atom, a)) {
	HeuristicValue v = heuristic_value(a);
	value = min(value, v);
	if (value.zero()) {
	  return value;
	}
      }
    }
    return value;
  }
}


/* Fills the provided list with actions that achieve the given
   formula. */
void PlanningGraph::achieves_formula(ActionList& actions,
				     const Literal& f) const {
  pair<LiteralActionsMapIter, LiteralActionsMapIter> bounds =
    achieves.equal_range(&f);
  for (LiteralActionsMapIter i = bounds.first; i != bounds.second; i++) {
    actions.push_back((*i).second);
  }
}


/* Returns the parameter domain for the given action, or NULL if the
   parameter domain is empty. */
const ActionDomain* PlanningGraph::action_domain(const string& name) const {
  ActionDomainMapIter di = action_domains.find(name);
  return (di != action_domains.end()) ? (*di).second : NULL;
}


/* ====================================================================== */
/* InvalidHeuristic */

/* Constructs an invalid heuristic exception. */
InvalidHeuristic::InvalidHeuristic(const string& name)
  : Exception("invalid heuristic `" + name + "'") {}


/* ====================================================================== */
/* Heuristic */

/* Selects a heuristic from a name. */
Heuristic& Heuristic::operator=(const string& name) {
  h_.clear();
  needs_pg_ = false;
  size_t pos = 0;
  while (pos < name.length()) {
    size_t next_pos = name.find(':', pos);
    string key = name.substr(pos, next_pos - pos);
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
    } else if (strcasecmp(n, "SUM") == 0) {
      h_.push_back(SUM);
      needs_pg_ = true;
    } else if (strcasecmp(n, "SUM_COST") == 0) {
      h_.push_back(SUM_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "SUM_WORK") == 0) {
      h_.push_back(SUM_WORK);
      needs_pg_ = true;
    } else if (strcasecmp(n, "SUMR") == 0) {
      h_.push_back(SUMR);
      needs_pg_ = true;
    } else if (strcasecmp(n, "SUMR_COST") == 0) {
      h_.push_back(SUMR_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "SUMR_WORK") == 0) {
      h_.push_back(SUMR_WORK);
      needs_pg_ = true;
    } else {
      throw InvalidHeuristic(name);
    }
    pos = (next_pos < name.length()) ? next_pos + 1 : next_pos;
  }
  return *this;
}


/* Checks if this heuristic needs a planning graph. */
bool Heuristic::needs_planning_graph() const {
  return needs_pg_;
}


/* Fills the provided vector with the ranks for the given plan. */
void Heuristic::plan_rank(vector<double>& rank, const Plan& plan,
			  double weight, const Domain& domain,
			  const PlanningGraph* planning_graph) const {
  bool sum_done = false;
  int sum_cost = 0;
  int sum_work = 0;
  bool sumr_done = false;
  int sumr_cost = 0;
  int sumr_work = 0;
  for (vector<HVal>::const_iterator hi = h_.begin(); hi != h_.end(); hi++) {
    HVal h = *hi;
    switch (h) {
    case LIFO:
      rank.push_back(-plan.serial_no());
      break;
    case FIFO:
      rank.push_back(plan.serial_no());
      break;
    case OC:
      rank.push_back(plan.num_open_conds);
      break;
    case UC:
      rank.push_back(plan.num_unsafes);
      break;
    case BUC:
      rank.push_back((plan.num_unsafes > 0) ? 1 : 0);
      break;
    case S_PLUS_OC:
      rank.push_back(plan.num_steps + weight*plan.num_open_conds);
      break;
    case UCPOP:
      rank.push_back(plan.num_steps
		     + weight*(plan.num_open_conds + plan.num_unsafes));
      break;
    case SUM:
    case SUM_COST:
    case SUM_WORK:
      if (!sum_done) {
	const Bindings* bindings = plan.bindings();
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  HeuristicValue v;
	  occ->head->condition().heuristic_value(v, *planning_graph, bindings);
	  sum_cost = sum(sum_cost, v.sum_cost());
	  sum_work = sum(sum_work, v.sum_work());
	}
      }
      if (h != SUM_WORK) {
	if (sum_cost < INT_MAX) {
	  rank.push_back(plan.num_steps + weight*sum_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      if (h != SUM_COST) {
	if (sum_work < INT_MAX) {
	  rank.push_back(sum_work);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      break;
    case SUMR:
    case SUMR_COST:
    case SUMR_WORK:
      if (!sumr_done) {
	const Bindings* bindings = plan.bindings();
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  bool reuse = false;
	  const LiteralOpenCondition* loc =
	    dynamic_cast<const LiteralOpenCondition*>(occ->head);
	  if (loc != NULL && !loc->is_static(domain)) {
	    hash_set<size_t> seen_steps;
	    for (const StepChain* sc = plan.steps;
		 sc != NULL && !reuse; sc = sc->tail) {
	      const Step& step = *sc->head;
	      if (seen_steps.find(step.id) == seen_steps.end()) {
		seen_steps.insert(step.id);
		if (plan.orderings.possibly_before(step.id,
						   loc->step_id)) {
		  const EffectList& effs = step.effects;
		  for (EffectListIter ei = effs.begin();
		       ei != effs.end() && !reuse; ei++) {
		    const AtomList& adds = (*ei)->add_list;
		    for (AtomListIter fi = adds.begin();
			 fi != adds.end() && !reuse; fi++) {
		      if ((bindings != NULL
			   && bindings->unify(loc->literal, **fi))
			  || (bindings == NULL
			      && Bindings::unifiable(loc->literal, **fi))) {
			sumr_work = sum(sumr_work, 1);
			reuse = true;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	  if (!reuse) {
	    HeuristicValue v;
	    occ->head->condition().heuristic_value(v, *planning_graph,
						   bindings);
	    sumr_cost = sum(sumr_cost, v.sum_cost());
	    sumr_work = sum(sumr_work, v.sum_work());
	  }
	}
      }
      if (h != SUMR_WORK) {
	if (sumr_cost < INT_MAX) {
	  rank.push_back(plan.num_steps + weight*sumr_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      if (h != SUMR_COST) {
	if (sumr_work < INT_MAX) {
	  rank.push_back(sumr_work);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      break;
    }
  }
}


/* Selects a flaw selection order from a name. */
FlawSelectionOrder& FlawSelectionOrder::operator=(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "MC_SUM") == 0) {
    primary_ = COST;
    extreme_ = MOST;
    heuristic_ = SUM;
  } else if (strcasecmp(n, "LC_SUM") == 0) {
    primary_ = COST;
    extreme_ = LEAST;
    heuristic_ = SUM;
  } else if (strcasecmp(n, "MW_SUM") == 0) {
    primary_ = WORK;
    extreme_ = MOST;
    heuristic_ = SUM;
  } else if (strcasecmp(n, "LW_SUM") == 0) {
    primary_ = WORK;
    extreme_ = LEAST;
    heuristic_ = SUM;
  } else if (strcasecmp(n, "LIFO") == 0) {
    secondary_ = LIFO;
  } else if (strcasecmp(n, "FIFO") == 0) {
    secondary_ = FIFO;
  } else if (strcasecmp(n, "RANDOM") == 0) {
    secondary_ = RANDOM;
  } else if (strcasecmp(n, "STATIC") == 0) {
    static_first_ = true;
  } else {
    throw InvalidFlawSelectionOrder(name);
  }
  return *this;
}


/* Checks if this flaw order needs a planning graph. */
bool FlawSelectionOrder::needs_planning_graph() const {
  return heuristic_ == SUM || heuristic_ == MAX;
}


/* Selects an open condition from the given list. */
const Flaw&
FlawSelectionOrder::select(const Plan& plan, const Domain& domain,
			   const PlanningGraph* pg) const {
  const UnsafeChain* unsafes = plan.unsafes;
  const OpenConditionChain* open_conds = plan.open_conds;
  const Bindings* bindings = plan.bindings();
  if (open_conds == NULL || (!static_first_ && unsafes != NULL)) {
    return *unsafes->head;
  }
  const OpenCondition* best_oc = open_conds->head;
  bool best_is_static = best_oc->is_static(domain);
  if (primary_ == NONE && secondary_ == LIFO
      && (!static_first_ || best_is_static)) {
    return *best_oc;
  }
  HeuristicValue best_value;
  if (pg != NULL) {
    best_oc->condition().heuristic_value(best_value, *pg, bindings);
  }
  if (verbosity > 2) {
    cout << endl << *best_oc << " with value " << best_value
	 << " (best)" << endl;
  }
  int streak = 1;
  for (const OpenConditionChain* oci = open_conds->tail;
       oci != NULL; oci = oci->tail) {
    const OpenCondition* oc = oci->head;
    bool is_static = oc->is_static(domain);
    bool better = false;
    bool equal = true;
    HeuristicValue value;
    if (heuristic_ != UNSPEC && primary_ != NONE) {
      oc->condition().heuristic_value(value, *pg, bindings);
      if (heuristic_ == MAX) {
	if (primary_ == COST) {
	  better = ((extreme_ == MOST)
		    ? (value.max_cost() > best_value.max_cost()
		       || (value.max_cost() == best_value.max_cost()
			   && value.max_work() > best_value.max_work()))
		    : (value.max_cost() < best_value.max_cost()
		       || (value.max_cost() == best_value.max_cost()
			   && value.max_work() < best_value.max_work())));
	  equal = (value.max_cost() == best_value.max_cost()
		   && value.max_work() == best_value.max_work());
	} else if (primary_ == WORK) {
	  better = ((extreme_ == MOST)
		    ? value.max_work() > best_value.max_work()
		    : value.max_work() < best_value.max_work());
	  equal = value.max_work() == best_value.max_work();
	}
      } else if (heuristic_ == SUM) {
	if (primary_ == COST) {
	  better = ((extreme_ == MOST)
		    ? (value.sum_cost() > best_value.sum_cost()
		       || (value.sum_cost() == best_value.sum_cost()
			   && value.sum_work() > best_value.sum_work()))
		    : (value.sum_cost() < best_value.sum_cost()
		       || (value.sum_cost() == best_value.sum_cost()
			   && value.sum_work() < best_value.sum_work())));
	  equal = (value.sum_cost() == best_value.sum_cost()
		   && value.sum_work() == best_value.sum_work());
	} else if (primary_ == WORK) {
	  better = ((extreme_ == MOST)
		    ? value.sum_work() > best_value.sum_work()
		    : value.sum_work() < best_value.sum_work());
	  equal = value.sum_work() == best_value.sum_work();
	}
      }
    }
    if (static_first_) {
      if (is_static && !best_is_static) {
	better = true;
	equal = false;
      } else if (!is_static && best_is_static) {
	better = false;
	equal = false;
      }
    }
    if (verbosity > 2) {
      cout << *oc << " with value " << value;
    }
    if (equal) {
      streak++;
    } else {
      streak = 1;
    }
    if (better
	|| (equal && (secondary_ == FIFO
		      || (secondary_ == RANDOM && rand01ex() < 1.0/streak)))) {
      if (verbosity > 2) {
	cout << " (best)" << endl;
      }
      best_oc = oc;
      best_value = value;
      best_is_static = is_static;
    } else if (verbosity > 2) {
      cout << endl;
    }
  }
  if (!best_is_static && unsafes != NULL) {
    return *unsafes->head;
  } else {
    return *best_oc;
  }
}


/* Prints this object on the given stream. */
void FlawSelectionOrder::print(ostream& os) const {
  switch (heuristic_) {
  case UNSPEC:
    cout << "UNSPEC";
    break;
  case MAX:
    cout << "MAX";
    break;
  case SUM:
    cout << "SUM";
    break;
  }
  cout << ':';
  switch (primary_) {
  case NONE:
    cout << "NONE";
    break;
  case COST:
    cout << "COST";
    break;
  case WORK:
    cout << "WORK";
    break;
  }
  cout << ':';
  switch (extreme_) {
  case MOST:
    cout << "MOST";
    break;
  case LEAST:
    cout << "LEAST";
    break;
  }
  cout << ':';
  switch (secondary_) {
  case LIFO:
    cout << "LIFO";
    break;
  case FIFO:
    cout << "FIFO";
    break;
  case RANDOM:
    cout << "RANDOM";
    break;
  }
  cout << endl;
}
