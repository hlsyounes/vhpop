/*
 * $Id: heuristics.cc,v 1.1 2001-09-29 18:54:53 lorens Exp $
 */
#include <hash_map>
#include "heuristics.h"
#include "formulas.h"
#include "domains.h"
#include "problems.h"


const Cost Cost::INFINITE = Cost(INT_MAX, INT_MAX);


/* Prints this object on the given stream. */
void InvalidHeuristic::print(ostream& os) const {
  os << "invalid heuristic `" << name << "'";
}


/* Prints this object on the given stream. */
void InvalidFlawSelectionOrder::print(ostream& os) const {
  os << "invalid flaw selection order `" << name << "'";
}


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


void Heuristic::compute_cost(hash_map<const Formula*, Cost>& atom_cost,
			     const Problem& problem,
			     const ActionList& actions) const {
  FormulaList goals;
  FormulaList neg_goals;
  if (problem.init != NULL) {
    problem.init->achievable_goals(goals, neg_goals);
    for (FLCI fi = goals.begin(); fi != goals.end(); fi++) {
      const AtomicFormula& atom = dynamic_cast<const AtomicFormula&>(**fi);
      if (!problem.domain.static_predicate(atom.predicate)) {
	atom_cost[&atom] = Cost(0, 1);
      }
    }
  }
  bool changed;
  do {
    changed = false;
    hash_map<const Formula*, Cost> new_costs;
    for (ActionList::const_iterator ai = actions.begin();
	 ai != actions.end(); ai++) {
      const Action& a = **ai;
      Cost c = a.precondition.cost(atom_cost, *this);
      if (!c.infinite()) {
	c.cost++;
	c.work++;
	goals.clear();
	neg_goals.clear();
	a.achievable_goals(goals, neg_goals);
	for (FLCI fi = goals.begin(); fi != goals.end(); fi++) {
	  hash_map<const Formula*, Cost>::const_iterator ci =
	    new_costs.find(*fi);
	  if (ci == new_costs.end() || c < (*ci).second) {
	    ci = atom_cost.find(*fi);
	    if (ci == atom_cost.end() || c < (*ci).second) {
	      new_costs[*fi] = c;
	      changed = true;
	    }
	  }
	}
      }
    }
    for (hash_map<const Formula*, Cost>::const_iterator ci = new_costs.begin();
	 ci != new_costs.end(); ci++) {
      atom_cost[(*ci).first] = (*ci).second;
    }
  } while (changed);
}


/* Selects a flaw selection order from a name. */
FlawSelectionOrder& FlawSelectionOrder::operator=(const string& name) {
  const char* n = name.c_str();
  if (strcasecmp(n, "LIFO") == 0) {
    set_lifo();
  } else if (strcasecmp(n, "FIFO") == 0) {
    set_fifo();
  } else if (strcasecmp(n, "MC") == 0) {
    set_most_cost_first();
  } else if (strcasecmp(n, "LC") == 0) {
    set_least_cost_first();
  } else if (strcasecmp(n, "MW") == 0) {
    set_most_work_first();
  } else if (strcasecmp(n, "LW") == 0) {
    set_least_work_first();
  } else if (strcasecmp(n, "ML") == 0) {
    set_most_linkable_first();
  } else if (strcasecmp(n, "LL") == 0) {
    set_least_linkable_first();
  } else {
    throw InvalidFlawSelectionOrder(name);
  }
  return *this;
}
