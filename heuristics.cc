/*
 * $Id: heuristics.cc,v 1.6 2001-10-18 21:16:03 lorens Exp $
 */
#include <set>
#include "heuristics.h"
#include "formulas.h"
#include "domains.h"
#include "problems.h"
#include "debug.h"
#include "plans.h"


/* Selects a heuristic from a name. */
Heuristic& Heuristic::operator=(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "MAX") == 0) {
    set_max();
  } else if (strcasecmp(n, "SUM") == 0) {
    set_sum();
  } else if (strcasecmp(n, "SUMR") == 0) {
    set_sum_reuse();
  } else if (strcasecmp(n, "UCPOP") == 0) {
    set_ucpop();
  } else {
    throw InvalidHeuristic(name);
  }
  return *this;
}


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


/* Returns the heuristic value of this formula. */
HeuristicValue Atom::heuristic_value(const PlanningGraph& pg,
				     const Bindings* b) const {
  return pg.heuristic_value(*this, b);
}


/* Returns the heuristic value of this formula. */
HeuristicValue Negation::heuristic_value(const PlanningGraph& pg,
					 const Bindings* b) const {
  return pg.heuristic_value(*this, b);
}


/* Returns the heuristic vaue of this formula. */
HeuristicValue Equality::heuristic_value(const PlanningGraph& pg,
					 const Bindings* b) const {
  if (b == NULL) {
    return HeuristicValue::ZERO;
  } else {
    return (b->consistent_with(*this)
	    ? HeuristicValue::ZERO : HeuristicValue::INFINITE);
  }
}


/* Returns the heuristic value of this formula. */
HeuristicValue Inequality::heuristic_value(const PlanningGraph& pg,
					   const Bindings* b) const {
  if (b == NULL) {
    return HeuristicValue::ZERO;
  } else {
    return (b->consistent_with(*this)
	    ? HeuristicValue::ZERO : HeuristicValue::INFINITE);
  }
}


/* Returns the heuristic value of this formula. */
HeuristicValue Conjunction::heuristic_value(const PlanningGraph& pg,
					    const Bindings* b) const {
  HeuristicValue value = HeuristicValue::ZERO;
  for (FormulaListIter fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    value += (*fi)->heuristic_value(pg, b);
    if (value.infinite()) {
      return value;
    }
  }
  return value;
}


/* Returns the heuristic value of this formula. */
HeuristicValue Disjunction::heuristic_value(const PlanningGraph& pg,
					    const Bindings* b) const {
  HeuristicValue value = HeuristicValue::INFINITE;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    value = min(value, (*fi)->heuristic_value(pg, b));
    if (value.zero()) {
      return value;
    }
  }
  return value;
}


/* Returns the heuristic value of this formula. */
HeuristicValue
QuantifiedFormula::heuristic_value(const PlanningGraph& pg,
				   const Bindings* b) const {
  throw Unimplemented("heuristic value of quantified formula not implemented");
}


/* Constructs a planning graph. */
PlanningGraph::PlanningGraph(const Problem& problem) {
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
    atom_values.insert(make_pair(*gi, HeuristicValue::ZERO_COST_UNIT_WORK));
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
      HeuristicValue pre_value = action.precondition.heuristic_value(*this);
      if (!pre_value.infinite()) {
	/* Precondition is achievable at this level. */
	if (applicable_actions.find(&action) == applicable_actions.end()) {
	  /* First time this action is applicable. */
	  applicable_actions.insert(&action);
	  if (!action.arguments.empty()) {
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
	  HeuristicValue cond_value = effect.condition.heuristic_value(*this);
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
    predicate_atoms.insert(make_pair(atom.predicate, &atom));
  }

  /*
   * Map predicates to achievable negated ground atoms.
   */
  for (AtomValueMapIter vi = negation_values.begin();
       vi != negation_values.end(); vi++) {
    const Atom& atom = *(*vi).first;
    predicate_negations.insert(make_pair(atom.predicate, &atom));
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
      predicate_atoms.equal_range(atom.predicate);
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
      predicate_negations.equal_range(negation.atom.predicate);
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
				     const Formula& f) const {
  pair<FormulaActionsMapIter, FormulaActionsMapIter> bounds =
    achieves.equal_range(&f);
  for (FormulaActionsMapIter i = bounds.first; i != bounds.second; i++) {
    actions.push_back((*i).second);
  }
}


/* Returns the parameter domain for the given action, or NULL if the
   parameter domain is empty. */
const ActionDomain* PlanningGraph::action_domain(const string& name) const {
  ActionDomainMapIter di = action_domains.find(name);
  return (di != action_domains.end()) ? (*di).second : NULL;
}


/* Selects a flaw selection order from a name. */
FlawSelectionOrder& FlawSelectionOrder::operator=(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "MAX") == 0) {
    heuristic_ = MAX;
  } else if (strcasecmp(n, "SUM") == 0 || strcasecmp(n, "SUMR") == 0) {
    heuristic_ = SUM;
  } else if (strcasecmp(n, "UCPOP") == 0) {
  } else if (strcasecmp(n, "MC") == 0) {
    primary_ = COST;
    extreme_ = MOST;
  } else if (strcasecmp(n, "LC") == 0) {
    primary_ = COST;
    extreme_ = LEAST;
  } else if (strcasecmp(n, "MW") == 0) {
    primary_ = WORK;
    extreme_ = MOST;
  } else if (strcasecmp(n, "LW") == 0) {
    primary_ = WORK;
    extreme_ = LEAST;
  } else if (strcasecmp(n, "LIFO") == 0) {
    secondary_ = LIFO;
  } else if (strcasecmp(n, "FIFO") == 0) {
    secondary_ = FIFO;
  } else if (strcasecmp(n, "RANDOM") == 0) {
    secondary_ = RANDOM;
  } else {
    throw InvalidFlawSelectionOrder(name);
  }
  return *this;
}


/* Sets the flaw value heuristic, but only if it is unspecified. */
void FlawSelectionOrder::set_heuristic(const Heuristic& h) {
  if (heuristic_ == UNSPEC) {
    if (h.max()) {
      heuristic_ = MAX;
    } else if (h.sum() || h.sum_reuse()) {
      heuristic_ = SUM;
    }
  } else if (h.ucpop()) {
    primary_ = NONE;
  }
}


/* Selects an open condition from the given list. */
const OpenCondition*
FlawSelectionOrder::select(const Chain<const OpenCondition*>* open_conds,
			   const PlanningGraph& pg,
			   const Bindings* bindings) const {
  if (open_conds == NULL) {
    return NULL;
  }
  const OpenCondition* best_oc = open_conds->head;
  HeuristicValue best_value = best_oc->condition.heuristic_value(pg, bindings);
  int streak = 1;
  for (const OpenConditionChain* oci = open_conds->tail;
       oci != NULL; oci = oci->tail) {
    const OpenCondition* oc = oci->head;
    bool better = false;
    bool equal = true;
    HeuristicValue value;
    if (heuristic_ != UNSPEC && primary_ != NONE) {
      value = oc->condition.heuristic_value(pg, bindings);
      if (heuristic_ == MAX) {
	if (primary_ == COST) {
	  better = ((extreme_ == MOST)
		    ? value.max_cost() > best_value.max_cost()
		    : value.max_cost() < best_value.max_cost());
	  equal = value.max_cost() == best_value.max_cost();
	} else if (primary_ == WORK) {
	  better = ((extreme_ == MOST)
		    ? value.max_work() > best_value.max_work()
		    : value.max_work() < best_value.max_work());
	  equal = value.max_work() == best_value.max_work();
	}
      } else if (heuristic_ == SUM) {
	if (primary_ == COST) {
	  better = ((extreme_ == MOST)
		    ? value.sum_cost() > best_value.sum_cost()
		    : value.sum_cost() < best_value.sum_cost());
	  equal = value.sum_cost() == best_value.sum_cost();
	} else if (primary_ == WORK) {
	  better = ((extreme_ == MOST)
		    ? value.sum_work() > best_value.sum_work()
		    : value.sum_work() < best_value.sum_work());
	  equal = value.sum_work() == best_value.sum_work();
	}
      }
    }
    if (equal) {
      streak++;
    } else {
      streak = 1;
    }
    if (better
	|| (equal && (secondary_ == FIFO
		      || (secondary_ == RANDOM && drand48() < 1.0/streak)))) {
      best_oc = oc;
      best_value = value;
    }
  }
  return best_oc;
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
