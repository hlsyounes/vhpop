/*
 * $Id: heuristics.cc,v 1.23 2002-01-07 19:48:18 lorens Exp $
 */
#include <set>
#include <typeinfo>
#include <climits>
#include <cmath>
#include "heuristics.h"
#include "plans.h"
#include "bindings.h"
#include "orderings.h"
#include "flaws.h"
#include "problems.h"
#include "domains.h"
#include "debug.h"


/* Returns the sum of two integers, avoiding overflow. */
static int sum(int n, int m) {
  return (INT_MAX - n > m) ? n + m : INT_MAX;
}


/* Computes the heuristic value of the given formula. */
static HeuristicValue
formula_value(const Formula& formula, size_t step_id, const Plan& plan,
	      const Domain& domain, const PlanningGraph& pg,
	      bool reuse = false) {
  HeuristicValue h;
  const Bindings* bindings = plan.bindings();
  bool has_reuse = false;
  if (reuse) {
    const Literal* literal = dynamic_cast<const Literal*>(&formula);
    if (literal != NULL) {
      if (!domain.static_predicate(literal->predicate())) {
	hash_set<size_t> seen_steps;
	for (const StepChain* sc = plan.steps;
	     sc != NULL && !has_reuse; sc = sc->tail) {
	  const Step& step = *sc->head;
	  if (step.id != 0 && seen_steps.find(step.id) == seen_steps.end()) {
	    seen_steps.insert(step.id);
	    if (plan.orderings.possibly_before(step.id, step_id)) {
	      const EffectList& effs = step.effects;
	      for (EffectListIter ei = effs.begin();
		   ei != effs.end() && !has_reuse; ei++) {
		if (typeid(*literal) == typeid(Atom)) {
		  const AtomList& adds = (*ei)->add_list;
		  for (AtomListIter fi = adds.begin();
		       fi != adds.end() && !has_reuse; fi++) {
		    if ((bindings != NULL && bindings->unify(*literal, **fi))
			|| (bindings == NULL
			    && Bindings::unifiable(*literal, **fi))) {
		      h = HeuristicValue::ZERO_COST_UNIT_WORK;
		      has_reuse = true;
		    }
		  }
		} else {
		  const NegationList& dels = (*ei)->del_list;
		  for (NegationListIter fi = dels.begin();
		       fi != dels.end() && !has_reuse; fi++) {
		    if ((bindings != NULL && bindings->unify(*literal, **fi))
			|| (bindings == NULL
			    && Bindings::unifiable(*literal, **fi))) {
		      h = HeuristicValue::ZERO_COST_UNIT_WORK;
		      has_reuse = true;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    } else {
      // reuse for disjunctions and conjunctions ?
    }
  }
  if (!has_reuse) {
    formula.heuristic_value(h, pg, bindings);
  }
  return h;
}


/* ====================================================================== */
/* HeuristicValue */

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

/* Constructs a heuristic from a name. */
Heuristic::Heuristic(const string& name) {
  *this = name;
}


/* Selects a heuristic from a name. */
Heuristic& Heuristic::operator=(const string& name) {
  h_.clear();
  needs_pg_ = false;
  size_t pos = 0;
  while (pos < name.length()) {
    size_t next_pos = name.find('/', pos);
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
    } else if (strcasecmp(n, "MAX") == 0) {
      h_.push_back(MAX);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAX_COST") == 0) {
      h_.push_back(MAX_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAX_WORK") == 0) {
      h_.push_back(MAX_WORK);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAXR") == 0) {
      h_.push_back(MAXR);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAXR_COST") == 0) {
      h_.push_back(MAXR_COST);
      needs_pg_ = true;
    } else if (strcasecmp(n, "MAXR_WORK") == 0) {
      h_.push_back(MAXR_WORK);
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
void Heuristic::plan_rank(vector<double>& rank, const Plan& plan,
			  double weight, const Domain& domain,
			  const PlanningGraph* planning_graph) const {
  bool sum_done = false;
  int sum_cost = 0;
  int sum_work = 0;
  bool sumr_done = false;
  int sumr_cost = 0;
  int sumr_work = 0;
  bool max_done = false;
  int max_cost = 0;
  int max_work = 0;
  bool maxr_done = false;
  int maxr_cost = 0;
  int maxr_work = 0;
  int max_steps = 0;
  for (vector<HVal>::const_iterator hi = h_.begin(); hi != h_.end(); hi++) {
    HVal h = *hi;
    switch (h) {
    case LIFO:
      rank.push_back(-1.0*plan.serial_no());
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
	sum_done = true;
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = *occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.step_id, plan,
			  domain, *planning_graph);
	  sum_cost = sum(sum_cost, v.sum_cost());
	  sum_work = sum(sum_work, v.sum_work());
	}
      }
      if (h == SUM) {
	if (sum_cost < INT_MAX) {
	  rank.push_back(plan.num_steps + weight*sum_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else if (h == SUM_COST) {
	if (sum_cost < INT_MAX) {
	  rank.push_back(sum_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else {
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
	sumr_done = true;
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = *occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.step_id, plan,
			  domain, *planning_graph, true);
	  sumr_cost = sum(sumr_cost, v.sum_cost());
	  sumr_work = sum(sumr_work, v.sum_work());
	}
      }
      if (h == SUMR) {
	if (sumr_cost < INT_MAX) {
	  rank.push_back(plan.num_steps + weight*sumr_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else if (h == SUMR_COST) {
	if (sumr_cost < INT_MAX) {
	  rank.push_back(sumr_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else {
	if (sumr_work < INT_MAX) {
	  rank.push_back(sumr_work);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      break;
    case MAX:
    case MAX_COST:
    case MAX_WORK:
      if (!max_done) {
	max_done = true;
	hash_map<size_t, size_t> dist;
	max_cost = max_steps = plan.orderings.goal_distances(dist);
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = *occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.step_id, plan,
			  domain, *planning_graph);
	  max_cost = max(max_cost,
			 int(dist[open_cond.step_id]) + v.max_cost());
	  max_work = sum(max_work, v.max_work());
	}
      }
      if (h == MAX) {
	if (max_cost < INT_MAX) {
	  rank.push_back(max_steps + weight*(max_cost - max_steps));
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else if (h == MAX_COST) {
	if (max_cost < INT_MAX) {
	  rank.push_back(max_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else {
	if (max_work < INT_MAX) {
	  rank.push_back(max_work);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      break;
    case MAXR:
    case MAXR_COST:
    case MAXR_WORK:
      if (!maxr_done) {
	maxr_done = true;
	hash_map<size_t, size_t> dist;
	maxr_cost = max_steps = plan.orderings.goal_distances(dist);
	for (const OpenConditionChain* occ = plan.open_conds;
	     occ != NULL; occ = occ->tail) {
	  const OpenCondition& open_cond = *occ->head;
	  HeuristicValue v =
	    formula_value(open_cond.condition(), open_cond.step_id, plan,
			  domain, *planning_graph, true);
	  maxr_cost = max(maxr_cost,
			  int(dist[open_cond.step_id]) + v.max_cost());
	  maxr_work = sum(maxr_work, v.max_work());
	}
      }
      if (h == MAXR) {
	if (maxr_cost < INT_MAX) {
	  rank.push_back(max_steps + weight*(maxr_cost - max_steps));
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else if (h == MAXR_COST) {
	if (maxr_cost < INT_MAX) {
	  rank.push_back(maxr_cost);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      } else {
	if (maxr_work < INT_MAX) {
	  rank.push_back(maxr_work);
	} else {
	  rank.push_back(HUGE_VAL);
	}
      }
      break;
    }
  }
}


/* ====================================================================== */
/* InvalidFlawSelectionOrder */

/* Constructs an invalid flaw selection order exception. */
InvalidFlawSelectionOrder::InvalidFlawSelectionOrder(const string& name)
  : Exception("invalid flaw selection order `" + name + "'") {}


/* ====================================================================== */
/* FlawSelectionOrder */


/* Output operator for selection criterion. */
ostream& operator<<(ostream& os, const SelectionCriterion& c) {
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
    case SelectionCriterion::SUM:
      os << "SUM";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAX:
      os << "MAX";
      if (c.reuse) {
	os << 'R';
      }
      break;
    }
    break;
  case SelectionCriterion::MC:
    os << "MC_";
    switch (c.heuristic) {
    case SelectionCriterion::SUM:
      os << "SUM";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAX:
      os << "MAX";
      if (c.reuse) {
	os << 'R';
      }
      break;
    }
    break;
  case SelectionCriterion::LW:
    os << "LW_";
    switch (c.heuristic) {
    case SelectionCriterion::SUM:
      os << "SUM";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAX:
      os << "MAX";
      if (c.reuse) {
	os << 'R';
      }
      break;
    }
    break;
  case SelectionCriterion::MW:
    os << "MW_";
    switch (c.heuristic) {
    case SelectionCriterion::SUM:
      os << "SUM";
      if (c.reuse) {
	os << 'R';
      }
      break;
    case SelectionCriterion::MAX:
      os << "MAX";
      if (c.reuse) {
	os << 'R';
      }
      break;
    }
    break;
  }
  return os;
}


/* Constructs a default flaw selection order. */
FlawSelectionOrder::FlawSelectionOrder(const string& name) {
  *this = name;
}


/* Selects a flaw selection order from a name. */
FlawSelectionOrder& FlawSelectionOrder::operator=(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "UCPOP") == 0) {
    return *this = "{n,s}LIFO/{o}LIFO";
  } else if (strcasecmp(n, "LCFR") == 0) {
    return *this = "{n,s,o}LR";
  } else if (strcasecmp(n, "ZLIFO") == 0) {
    return *this = "{n}LIFO/{o}0LIFO/{o}1NEW/{o}LIFO/{s}LIFO";
  } else if (strcasecmp(n, "STATIC") == 0) {
    return *this = "{t}LIFO/{n,s}LIFO/{o}LIFO";
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
      string number = name.substr(pos, next_pos - pos);
      criterion.max_refinements = atoi(number.c_str());
      pos = next_pos;
    } else {
      criterion.max_refinements = INT_MAX;
    }
    next_pos = name.find('/', pos);
    string key = name.substr(pos, next_pos - pos);
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
	if (strcasecmp(n + 3, "SUM") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "SUMR") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAX") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "MAXR") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "MC_", 3) == 0) {
	criterion.order = SelectionCriterion::MC;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "SUM") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "SUMR") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAX") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "MAXR") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "LW_", 3) == 0) {
	criterion.order = SelectionCriterion::LW;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "SUM") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "SUMR") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAX") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "MAXR") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else if (strncasecmp(n, "MW_", 3) == 0) {
	criterion.order = SelectionCriterion::MW;
	needs_pg_ = true;
	if (strcasecmp(n + 3, "SUM") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "SUMR") == 0) {
	  criterion.heuristic = SelectionCriterion::SUM;
	  criterion.reuse = true;
	} else if (strcasecmp(n + 3, "MAX") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = false;
	} else if (strcasecmp(n + 3, "MAXR") == 0) {
	  criterion.heuristic = SelectionCriterion::MAX;
	  criterion.reuse = true;
	} else {
	  throw InvalidFlawSelectionOrder(name);
	}
      } else {
	throw InvalidFlawSelectionOrder(name);
      }
    }
    if (criterion.non_separable) {
      non_separable_max_refinements = max(criterion.max_refinements,
					  non_separable_max_refinements);
    }
    if (criterion.separable) {
      separable_max_refinements = max(criterion.max_refinements,
				      separable_max_refinements);
    }
    if (criterion.open_cond || criterion.local_open_cond) {
      open_cond_max_refinements = max(criterion.max_refinements,
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


/* Counts the number of refinements for the given threat, and returns
   true iff the number of refinements does not exceed the given
   limit. */
static bool unsafe_refinements(int& refinements, int& separable,
			       int& promotable, int& demotable,
			       const Unsafe& unsafe, const Plan& plan,
			       int limit) {
  if (refinements >= 0) {
    return refinements <= limit;
  } else {
    int ref = 0;
    if (separable < 0) {
      separable = plan.separable(unsafe);
      if (separable < 0) {
	refinements = separable = 0;
	return true;
      }
    }
    ref += separable;
    if (ref <= limit) {
      if (promotable < 0) {
	promotable = plan.promotable(unsafe);
      }
      ref += promotable;
      if (ref <= limit) {
	if (demotable < 0) {
	  demotable = plan.demotable(unsafe);
	}
	refinements = ref + demotable;
	return refinements <= limit;
      }
    }
  }
  return false;
}


/* Seaches threats for a flaw to select. */
int FlawSelectionOrder::select_unsafe(FlawSelection& selection,
				      const Plan& plan, int first_criterion,
				      int last_criterion) const {
  if (first_criterion > last_criterion || plan.unsafes == NULL) {
    return INT_MAX;
  }
  /* Loop through usafes. */
  for (const UnsafeChain* uc = plan.unsafes;
       uc != NULL && first_criterion <= last_criterion; uc = uc->tail) {
    const Unsafe& unsafe = *uc->head;
    if (verbosity > 1) {
      cout << "(considering " << unsafe << ")" << endl;
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
	    || unsafe_refinements(refinements, separable, promotable,
				  demotable, unsafe, plan,
				  criterion.max_refinements)) {
	  /* Refinement constraint is satisfied, so criterion applies. */
	  switch (criterion.order) {
	  case SelectionCriterion::LIFO:
	    selection.flaw = &unsafe;
	    selection.criterion = c;
	    last_criterion = c - 1;
	    if (verbosity > 1) {
	      cout << "selecting " << unsafe << " by criterion "
		   << criterion << endl;
	    }
	    break;
	  case SelectionCriterion::FIFO:
	    selection.flaw = &unsafe;
	    selection.criterion = c;
	    last_criterion = c;
	    if (verbosity > 1) {
	      cout << "selecting " << unsafe << " by criterion "
		   << criterion << endl;
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
		cout << "selecting " << unsafe << " by criterion "
		     << criterion << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::LR:
	    if (c < selection.criterion
		|| unsafe_refinements(refinements, separable, promotable,
				      demotable, unsafe, plan,
				      selection.rank - 1)) {
	      selection.flaw = &unsafe;
	      selection.criterion = c;
	      unsafe_refinements(refinements, separable, promotable,
				 demotable, unsafe, plan, INT_MAX);
	      selection.rank = refinements;
	      last_criterion = (refinements == 0) ? c - 1 : c;
	      if (verbosity > 1) {
		cout << "selecting " << unsafe << " by criterion "
		     << criterion << " with rank " << refinements << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::MR:
	    unsafe_refinements(refinements, separable, promotable,
			       demotable, unsafe, plan, INT_MAX);
	    if (c < selection.criterion
		|| refinements > selection.rank) {
	      selection.flaw = &unsafe;
	      selection.criterion = c;
	      selection.rank = refinements;
	      last_criterion = (refinements == 3) ? c - 1 : c;
	      if (verbosity > 1) {
		cout << "selecting " << unsafe << " by criterion "
		     << criterion << " with rank " << refinements << endl;
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


/* Counts the number of refinements for the given open condition, and
   returns true iff the number of refinements does not exceed the
   given limit. */
static bool open_cond_refinements(int& refinements,
				  int& addable, int& reusable,
				  const OpenCondition& open_cond,
				  const Plan& plan, int limit) {
  if (refinements >= 0) {
    return refinements <= limit;
  } else {
    const LiteralOpenCondition* loc =
      dynamic_cast<const LiteralOpenCondition*>(&open_cond);
    if (loc != NULL) {
      int ref = 0;
      if (addable < 0) {
	if (!plan.addable_steps(addable, *loc, limit)) {
	  return false;
	}
      }
      ref += addable;
      if (ref <= limit) {
	if (reusable < 0) {
	  if (!plan.reusable_steps(reusable, *loc, limit)) {
	    return false;
	  }
	}
	refinements = ref + reusable;
	return refinements <= limit;
      }
    } else {
      const DisjunctiveOpenCondition* disjoc =
	dynamic_cast<const DisjunctiveOpenCondition*>(&open_cond);
      if (disjoc != NULL) {
	refinements = plan.disjunction_refinements(*disjoc);
	return refinements <= limit;
      } else {
	const InequalityOpenCondition* neqoc =
	  dynamic_cast<const InequalityOpenCondition*>(&open_cond);
	if (neqoc != NULL) {
	  refinements = plan.inequality_refinements(*neqoc);
	} else {
	  throw Unimplemented("unknown kind of open condition");
	}
      }
    }
  }
  return false;
}


/* Seaches open conditions for a flaw to select. */
int FlawSelectionOrder::select_open_cond(FlawSelection& selection,
					 const Plan& plan,
					 const Domain& domain,
					 const PlanningGraph* pg,
					 int first_criterion,
					 int last_criterion) const {
  if (first_criterion > last_criterion || plan.open_conds == NULL) {
    return INT_MAX;
  }
  size_t local_id = 0;
  /* Loop through open conditions. */
  for (const OpenConditionChain* occ = plan.open_conds;
       occ != NULL && first_criterion <= last_criterion; occ = occ->tail) {
    const OpenCondition& open_cond = *occ->head;
    if (verbosity > 1) {
      cout << "(considering " << open_cond << ")" << endl;
    }
    if (local_id == 0) {
      local_id = open_cond.step_id;
    }
    bool local = (open_cond.step_id == local_id);
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
	is_static = open_cond.is_static(domain) ? 1 : 0;
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
	    || open_cond_refinements(refinements, addable, reusable, open_cond,
				     plan, criterion.max_refinements)) {
	  /* Refinement constraint is satisfied, so criterion applies. */
	  switch (criterion.order) {
	  case SelectionCriterion::LIFO:
	    selection.flaw = &open_cond;
	    selection.criterion = c;
	    last_criterion = c - 1;
	    if (verbosity > 1) {
	      cout << "selecting " << open_cond << " by criterion "
		   << criterion << endl;
	    }
	    break;
	  case SelectionCriterion::FIFO:
	    selection.flaw = &open_cond;
	    selection.criterion = c;
	    last_criterion = c;
	    if (verbosity > 1) {
	      cout << "selecting " << open_cond << " by criterion "
		   << criterion << endl;
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
		cout << "selecting " << open_cond << " by criterion "
		     << criterion << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::LR:
	    if (c < selection.criterion
		|| open_cond_refinements(refinements, addable, reusable,
					 open_cond, plan,
					 selection.rank - 1)) {
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      open_cond_refinements(refinements, addable, reusable, open_cond,
				    plan, INT_MAX);
	      selection.rank = refinements;
	      last_criterion = (refinements == 0) ? c - 1 : c;
	      if (verbosity > 1) {
		cout << "selecting " << open_cond << " by criterion "
		     << criterion << " with rank " << refinements << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::MR:
	    open_cond_refinements(refinements, addable, reusable, open_cond,
				  plan, INT_MAX);
	    if (c < selection.criterion
		|| refinements > selection.rank) {
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      selection.rank = refinements;
	      last_criterion = c;
	      if (verbosity > 1) {
		cout << "selecting " << open_cond << " by criterion "
		     << criterion << " with rank " << refinements << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::NEW:
	    {
	      bool has_new = false;
	      if (addable < 0) {
		const LiteralOpenCondition* loc =
		  dynamic_cast<const LiteralOpenCondition*>(&open_cond);
		if (loc != NULL) {
		  has_new = !plan.addable_steps(addable, *loc, 0);
		}
	      } else {
		has_new = (addable > 0);
	      }
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      last_criterion = has_new ? c - 1 : c;
	      if (verbosity > 1) {
		cout << "selecting " << open_cond << " by criterion "
		     << criterion;
		if (has_new) {
		  cout << " with new";
		}
		cout << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::REUSE:
	    {
	      bool has_reuse = false;
	      if (reusable < 0) {
		const LiteralOpenCondition* loc =
		  dynamic_cast<const LiteralOpenCondition*>(&open_cond);
		if (loc != NULL) {
		  has_reuse = !plan.reusable_steps(reusable, *loc, 0);
		}
	      } else {
		has_reuse = (reusable > 0);
	      }
	      selection.flaw = &open_cond;
	      selection.criterion = c;
	      last_criterion = has_reuse ? c - 1 : c;
	      if (verbosity > 1) {
		cout << "selecting " << open_cond << " by criterion "
		     << criterion;
		if (has_reuse) {
		  cout << " with reuse";
		}
		cout << endl;
	      }
	    }
	    break;
	  case SelectionCriterion::LC:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.step_id, plan,
			      domain, *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::SUM)
			  ? h.sum_cost() : h.max_cost());
	      if (c < selection.criterion || rank < selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = (rank == 0) ? c - 1 : c;
		if (verbosity > 1) {
		  cout << "selecting " << open_cond << " by criterion "
		       << criterion << " with rank " << rank << endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::MC:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.step_id, plan,
			      domain, *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::SUM)
			  ? h.sum_cost() : h.max_cost());
	      if (c < selection.criterion || rank > selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = c;
		if (verbosity > 1) {
		  cout << "selecting " << open_cond << " by criterion "
		       << criterion << " with rank " << rank << endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::LW:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.step_id, plan,
			      domain, *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::SUM)
			  ? h.sum_work() : h.max_work());
	      if (c < selection.criterion || rank < selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = (rank == 0) ? c - 1 : c;
		if (verbosity > 1) {
		  cout << "selecting " << open_cond << " by criterion "
		       << criterion << " with rank " << rank << endl;
		}
	      }
	    }
	    break;
	  case SelectionCriterion::MW:
	    {
	      HeuristicValue h =
		formula_value(open_cond.condition(), open_cond.step_id, plan,
			      domain, *pg, criterion.reuse);
	      int rank = ((criterion.heuristic == SelectionCriterion::SUM)
			  ? h.sum_work() : h.max_work());
	      if (c < selection.criterion || rank > selection.rank) {
		selection.flaw = &open_cond;
		selection.criterion = c;
		selection.rank = rank;
		last_criterion = c;
		if (verbosity > 1) {
		  cout << "selecting " << open_cond << " by criterion "
		       << criterion << " with rank " << rank << endl;
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
const Flaw& FlawSelectionOrder::select(const Plan& plan, const Domain& domain,
				       const PlanningGraph* pg) const {
  FlawSelection selection;
  selection.criterion = INT_MAX;
  int last_criterion = select_unsafe(selection, plan, first_unsafe_criterion_,
				     last_unsafe_criterion_);
  select_open_cond(selection, plan, domain, pg, first_open_cond_criterion_,
		   min(last_open_cond_criterion_, last_criterion));
  return *selection.flaw;
}