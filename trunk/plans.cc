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
 * $Id: plans.cc,v 6.14 2003-12-05 21:27:19 lorens Exp $
 */
#include "mathport.h"
#include "plans.h"
#include "heuristics.h"
#include "bindings.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "requirements.h"
#include "parameters.h"
#include "debug.h"
#include <algorithm>
#include <queue>
#include <typeinfo>
#include <climits>
#include <sys/time.h>


/*
 * Mapping of predicate names to achievers.
 */
struct PredicateAchieverMap : public std::map<Predicate, ActionEffectMap> {
};


/* Planning parameters. */
static const Parameters* params;
/* Domain of problem currently being solved. */
static const Domain* domain = NULL;
/* Problem currently being solved. */
static const Problem* problem = NULL;
/* Planning graph. */
static const PlanningGraph* planning_graph;
/* The goal action. */
static Action* goal_action;
/* Maps predicates to actions. */
static PredicateAchieverMap achieves_pred;
/* Maps negated predicates to actions. */
static PredicateAchieverMap achieves_neg_pred;
/* Whether last flaw was a static predicate. */
static bool static_pred_flaw;


/* ====================================================================== */
/* Link */

/* Constructs a causal link. */
Link::Link(size_t from_id, StepTime effect_time,
	   const OpenCondition& open_cond)
  : from_id_(from_id), effect_time_(effect_time), to_id_(open_cond.step_id()),
    condition_(open_cond.literal()), condition_time_(open_cond.when()) {
  Formula::register_use(condition_);
}


/* Constructs a causal link. */
Link::Link(const Link& l)
  : from_id_(l.from_id_), effect_time_(l.effect_time_), to_id_(l.to_id_),
    condition_(l.condition_), condition_time_(l.condition_time_) {
  Formula::register_use(condition_);
}


/* Deletes this causal link. */
Link::~Link() {
  Formula::unregister_use(condition_);
}


/* ====================================================================== */
/* Plan */

/*
 * Less than function object for plan pointers.
 */
namespace std {
  struct less<const Plan*>
    : public binary_function<const Plan*, const Plan*, bool> {
    /* Comparison function operator. */
    bool operator()(const Plan* p1, const Plan* p2) const {
      return *p1 < *p2;
    }
  };
}


/*
 * A plan queue.
 */
struct PlanQueue : public std::priority_queue<const Plan*> {
};


/* Id of goal step. */
const size_t Plan::GOAL_ID = UINT_MAX;


/* Adds goal to chain of open conditions, and returns true if and only
   if the goal is consistent. */
static bool add_goal(const Chain<OpenCondition>*& open_conds,
		     size_t& num_open_conds, BindingList& new_bindings,
		     const Formula& goal, FormulaTime when, size_t step_id,
		     bool test_only = false) {
  if (goal.tautology()) {
    return true;
  } else if (goal.contradiction()) {
    return false;
  }
  std::vector<const Formula*> goals(1, &goal);
  while (!goals.empty()) {
    const Formula* goal = goals.back();
    goals.pop_back();
    const Literal* l = dynamic_cast<const Literal*>(goal);
    if (l != NULL) {
      if (!test_only
	  && !(params->strip_static_preconditions()
	       && domain->predicates().static_predicate(l->predicate()))) {
	open_conds =
	  new Chain<OpenCondition>(OpenCondition(step_id, *goal, when),
				   open_conds);
      }
      num_open_conds++;
    } else {
      const Conjunction* conj = dynamic_cast<const Conjunction*>(goal);
      if (conj != NULL) {
	const FormulaList& gs = conj->conjuncts();
	for (FormulaList::const_iterator fi = gs.begin();
	     fi != gs.end(); fi++) {
	  if (params->random_open_conditions) {
	    size_t pos = size_t((goals.size() + 1.0)*rand()/(RAND_MAX + 1.0));
	    if (pos == goals.size()) {
	      goals.push_back(*fi);
	    } else {
	      const Formula* tmp = goals[pos];
	      goals[pos] = *fi;
	      goals.push_back(tmp);
	    }
	  } else {
	    goals.push_back(*fi);
	  }
	}
      } else {
	const Disjunction* disj = dynamic_cast<const Disjunction*>(goal);
	if (disj != NULL) {
	  if (!test_only) {
	    open_conds =
	      new Chain<OpenCondition>(OpenCondition(step_id, *goal, when),
				       open_conds);
	  }
	  num_open_conds++;
	} else {
	  const BindingLiteral* bl = dynamic_cast<const BindingLiteral*>(goal);
	  if (bl != NULL) {
	    bool is_eq = (typeid(*bl) == typeid(Equality));
	    bool added = false;
	    if (is_variable(bl->term1())) {
	      /* The first term is a variable. */
	      new_bindings.push_back(Binding(bl->term1(),
					     bl->step_id1(step_id),
					     bl->term2(),
					     bl->step_id2(step_id), is_eq));
	      added = true;
	    } else if (is_variable(bl->term2())) {
	      /* The second term is a variable. */
	      new_bindings.push_back(Binding(bl->term2(),
					     bl->step_id2(step_id),
					     bl->term1(),
					     bl->step_id1(step_id), is_eq));
	      added = true;
	    } else if ((is_eq && bl->term1() != bl->term2())
		       || (!is_eq && bl->term1() == bl->term2())) {
	      /* Both terms are names, and the binding is inconsistent. */
	      return false;
	    }
	    if (added) {
#ifdef BRANCH_ON_INEQUALITY
	      const Inequality* neq = dynamic_cast<const Inequality*>(bl);
	      if (params->domain_constraints
		  && neq != NULL
		  && is_variable(bl->term1) && is_variable(bl->term2)) {
		/* Both terms are variables, so handle specially. */
		if (!test_only) {
		  open_conds =
		    new Chain<OpenCondition>(OpenCondition(step_id, *neq,
							   when),
					     open_conds);
		}
		num_open_conds++;
		new_bindings.pop_back();
	      }
#endif
	    }
	  } else {
	    const Exists* exists = dynamic_cast<const Exists*>(goal);
	    if (exists != NULL) {
	      if (params->random_open_conditions) {
		size_t pos =
		  size_t((goals.size() + 1.0)*rand()/(RAND_MAX + 1.0));
		if (pos == goals.size()) {
		  goals.push_back(&exists->body());
		} else {
		  const Formula* tmp = goals[pos];
		  goals[pos] = &exists->body();
		  goals.push_back(tmp);
		}
	      } else {
		goals.push_back(&exists->body());
	      }
	    } else {
	      const Forall* forall = dynamic_cast<const Forall*>(goal);
	      if (forall != NULL) {
		const Formula& g = forall->universal_base(SubstitutionMap(),
							  *problem);
		if (params->random_open_conditions) {
		  size_t pos =
		    size_t((goals.size() + 1.0)*rand()/(RAND_MAX + 1.0));
		  if (pos == goals.size()) {
		    goals.push_back(&g);
		  } else {
		    const Formula* tmp = goals[pos];
		    goals[pos] = &g;
		    goals.push_back(tmp);
		  }
		} else {
		  goals.push_back(&g);
		}
	      } else {
		throw Exception("unknown kind of goal");
	      }
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Adds goal to chain of open conditions, and returns true if and only
   if the goal is consistent. */
static bool add_goal(const Chain<OpenCondition>*& open_conds,
		     size_t& num_open_conds, BindingList& new_bindings,
		     const Condition& goal, size_t step_id,
		     bool test_only = false) {
  return (add_goal(open_conds, num_open_conds, new_bindings,
		   goal.at_start(), AT_START, step_id, test_only)
	  && add_goal(open_conds, num_open_conds, new_bindings,
		      goal.over_all(), OVER_ALL, step_id, test_only)
	  && add_goal(open_conds, num_open_conds, new_bindings,
		      goal.at_end(), AT_END, step_id, test_only));
}


/* Returns a set of achievers for the given literal. */
static const ActionEffectMap* literal_achievers(const Literal& literal) {
  if (params->ground_actions) {
    return planning_graph->literal_achievers(literal);
  } else if (typeid(literal) == typeid(Atom)) {
    PredicateAchieverMap::const_iterator pai =
      achieves_pred.find(literal.predicate());
    return (pai != achieves_pred.end()) ? &(*pai).second : NULL;
  } else {
    PredicateAchieverMap::const_iterator pai =
      achieves_neg_pred.find(literal.predicate());
    return (pai != achieves_neg_pred.end()) ? &(*pai).second : NULL;
  }
}


/* Finds threats to the given link. */
static void link_threats(const Chain<Unsafe>*& unsafes, size_t& num_unsafes,
			 const Link& link, const Chain<Step>* steps,
			 const Orderings& orderings,
			 const Bindings& bindings) {
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition_time());
  for (const Chain<Step>* sc = steps; sc != NULL; sc = sc->tail) {
    const Step& s = sc->head;
    if (link.from_id() == s.id()
	|| (orderings.possibly_before(link.from_id(), lt1, s.id(), STEP_END)
	    && orderings.possibly_after(link.to_id(), lt2,
					s.id(), STEP_START))) {
      const EffectList& effects = s.action().effects();
      for (EffectList::const_iterator ei = effects.begin();
	   ei != effects.end(); ei++) {
	const Effect& e = **ei;
	if (!domain->requirements.durative_actions
	    && e.link_condition().contradiction()) {
	  continue;
	}
	StepTime et = end_time(e);
	if ((link.from_id() == s.id() && lt1 == et)
	    || orderings.possibly_before(link.from_id(), lt1, s.id(), et)
	    || orderings.possibly_after(link.to_id(), lt2, s.id(), et)) {
	  if (typeid(link.condition()) == typeid(Negation)) {
	    if (bindings.affects(e.literal(), s.id(),
				 link.condition(), link.to_id(),
				 problem->domain().types(),
				 problem->terms())) {
	      unsafes = new Chain<Unsafe>(Unsafe(link, s.id(), e, e.literal()),
					  unsafes);
	      num_unsafes++;
	    }
	  } else if (!(link.from_id() == s.id() && lt1 == et)) {
	    if (bindings.affects(e.literal(), s.id(),
				 link.condition(), link.to_id(),
				 problem->domain().types(),
				 problem->terms())) {
	      unsafes = new Chain<Unsafe>(Unsafe(link, s.id(), e, e.literal()),
					  unsafes);
	      num_unsafes++;
	    }
	  }
	}
      }
    }
  }
}


/* Finds the threats by the given step. */
static void step_threats(const Chain<Unsafe>*& unsafes, size_t& num_unsafes,
			 const Step& step, const Chain<Link>* links,
			 const Orderings& orderings,
			 const Bindings& bindings) {
  for (const Chain<Link>* lc = links; lc != NULL; lc = lc->tail) {
    const Link& l = lc->head;
    StepTime lt1 = l.effect_time();
    StepTime lt2 = end_time(l.condition_time());
    if (orderings.possibly_before(l.from_id(), lt1, step.id(), STEP_END)
	&& orderings.possibly_after(l.to_id(), lt2, step.id(), STEP_START)) {
      const EffectList& effects = step.action().effects();
      for (EffectList::const_iterator ei = effects.begin();
	   ei != effects.end(); ei++) {
	const Effect& e = **ei;
	if (!domain->requirements.durative_actions
	    && e.link_condition().contradiction()) {
	  continue;
	}
	StepTime et = end_time(e);
	if (orderings.possibly_before(l.from_id(), lt1, step.id(), et)
	    && orderings.possibly_after(l.to_id(), lt2, step.id(), et)) {
	  if (bindings.affects(e.literal(), step.id(),
			       l.condition(), l.to_id(),
			       problem->domain().types(), problem->terms())) {
	    unsafes = new Chain<Unsafe>(Unsafe(l, step.id(), e, e.literal()),
					unsafes);
	    num_unsafes++;
	  }
	}
      }
    }
  }
}


/* Returns binding constraints that make the given steps fully
   instantiated, or NULL if no consistent binding constraints can be
   found. */
static const Bindings* step_instantiation(const Chain<Step>* steps, size_t n,
					  const Bindings& bindings) {
  if (steps == NULL) {
    return &bindings;
  } else {
    const Step& step = steps->head;
    const ActionSchema* as = dynamic_cast<const ActionSchema*>(&step.action());
    if (as == NULL || as->parameters().size() <= n) {
      return step_instantiation(steps->tail, 0, bindings);
    } else {
      Variable v = as->parameters()[n];
      if (v != bindings.binding(v, step.id())) {
	return step_instantiation(steps, n + 1, bindings);
      } else {
	std::cout << "instantiating parameter " << n << " of ";
	as->print(std::cout, problem->terms(), step.id(), bindings);
	std::cout << std::endl;
	Type t = problem->domain().terms().type(v);
	const ObjectList& arguments = problem->compatible_objects(t);
	for (ObjectList::const_iterator oi = arguments.begin();
	     oi != arguments.end(); oi++) {
	  BindingList bl;
	  bl.push_back(Binding(v, step.id(), *oi, 0, true));
	  const Bindings* new_bindings =
	    bindings.add(bl, problem->domain().types(), problem->terms());
	  if (new_bindings != NULL) {
	    const Bindings* result = step_instantiation(steps, n + 1,
							*new_bindings);
	    if (result != new_bindings) {
	      delete new_bindings;
	    }
	    if (result != NULL) {
	      return result;
	    }
	  }
	}
	return NULL;
      }
    }
  }
}


/* Returns the initial plan representing the given problem, or NULL
   if initial conditions or goals of the problem are inconsistent. */
const Plan* Plan::make_initial_plan(const Problem& problem) {
  /*
   * Create goal of problem.
   */
  if (params->ground_actions) {
    goal_action = new GroundAction("", false);
    const Formula& goal_formula =
      problem.goal().instantiation(SubstitutionMap(), problem);
    goal_action->set_condition(Condition::make(goal_formula, AT_START));
  } else {
    goal_action = new ActionSchema("", false);
    goal_action->set_condition(Condition::make(problem.goal(), AT_START));
  }
  /* Chain of open conditions. */
  const Chain<OpenCondition>* open_conds = NULL;
  /* Number of open conditions. */
  size_t num_open_conds = 0;
  /* Bindings introduced by goal. */
  BindingList new_bindings;
  /* Add goals as open conditions. */
  if (!add_goal(open_conds, num_open_conds, new_bindings,
		goal_action->condition(), GOAL_ID)) {
    /* Goals are inconsistent. */
    Chain<OpenCondition>::register_use(open_conds);
    Chain<OpenCondition>::unregister_use(open_conds);
    return NULL;
  }
  /* Make chain of initial steps. */
  const Chain<Step>* steps =
    new Chain<Step>(Step(0, problem.init_action()),
		    new Chain<Step>(Step(GOAL_ID, *goal_action), NULL));
  size_t num_steps = 0;
  /* Variable bindings. */
  const Bindings* bindings = &Bindings::EMPTY;
  /* Step orderings. */
  const Orderings* orderings;
  if (domain->requirements.durative_actions) {
    const TemporalOrderings* to = new TemporalOrderings();
    /*
     * Add steps for timed initial literals.
     */
    for (TimedActionTable::const_iterator ai = problem.timed_actions().begin();
	 ai != problem.timed_actions().end(); ai++) {
      num_steps++;
      steps = new Chain<Step>(Step(num_steps, *(*ai).second), steps);
      const TemporalOrderings* tmp = to->refine((*ai).first, steps->head);
      delete to;
      if (tmp == NULL) {
	Chain<OpenCondition>::register_use(open_conds);
	Chain<OpenCondition>::unregister_use(open_conds);
	Chain<Step>::register_use(steps);
	Chain<Step>::unregister_use(steps);
	return NULL;
      }
      to = tmp;
    }
    orderings = to;
  } else {
    orderings = new BinaryOrderings();
  }
  /* Return initial plan. */
  return new Plan(steps, num_steps, NULL, 0, *orderings, *bindings,
		  NULL, 0, open_conds, num_open_conds, NULL);
}


/* Returns plan for given problem. */
const Plan* Plan::plan(const Problem& problem, const Parameters& p,
		       bool last_problem) {
  /* Set planning parameters. */
  params = &p;
  /* Set current domain. */
  domain = &problem.domain();
  ::problem = &problem;

  /*
   * Initialize planning graph and maps from predicates to actions.
   */
  bool need_pg = (params->ground_actions || params->domain_constraints
		  || params->heuristic.needs_planning_graph());
  for (size_t i = 0; !need_pg && i < params->flaw_orders.size(); i++) {
    if (params->flaw_orders[i].needs_planning_graph()) {
      need_pg = true;
    }
  }
  if (need_pg) {
    planning_graph = new PlanningGraph(problem, *params);
  } else {
    planning_graph = NULL;
  }
  if (!params->ground_actions) {
    achieves_pred.clear();
    achieves_neg_pred.clear();
    for (ActionSchemaMap::const_iterator ai = domain->actions().begin();
	 ai != domain->actions().end(); ai++) {
      const ActionSchema* as = (*ai).second;
      for (EffectList::const_iterator ei = as->effects().begin();
	   ei != as->effects().end(); ei++) {
	const Literal& literal = (*ei)->literal();
	if (typeid(literal) == typeid(Atom)) {
	  achieves_pred[literal.predicate()].insert(std::make_pair(as, *ei));
	} else {
	  achieves_neg_pred[literal.predicate()].insert(std::make_pair(as,
								       *ei));
	}
      }
    }
    const GroundAction& ia = problem.init_action();
    for (EffectList::const_iterator ei = ia.effects().begin();
	 ei != ia.effects().end(); ei++) {
      const Literal& literal = (*ei)->literal();
      achieves_pred[literal.predicate()].insert(std::make_pair(&ia, *ei));
    }
    for (TimedActionTable::const_iterator ai = problem.timed_actions().begin();
	 ai != problem.timed_actions().end(); ai++) {
      const GroundAction& action = *(*ai).second;
      for (EffectList::const_iterator ei = action.effects().begin();
	   ei != action.effects().end(); ei++) {
	const Literal& literal = (*ei)->literal();
	if (typeid(literal) == typeid(Atom)) {
	  achieves_pred[literal.predicate()].insert(std::make_pair(&action,
								   *ei));
	} else {
	  achieves_neg_pred[literal.predicate()].insert(std::make_pair(&action,
								       *ei));
	}
      }
    }
  }
  static_pred_flaw = false;

  /* Number of visited plan. */
  size_t num_visited_plans = 0;
  /* Number of generated plans. */
  size_t num_generated_plans = 0;
  /* Number of static preconditions encountered. */
  size_t num_static = 0;
  /* Number of dead ends encountered. */
  size_t num_dead_ends = 0;

  /* Generated plans for different flaw selection orders. */
  std::vector<size_t> generated_plans(params->flaw_orders.size(), 0);
  /* Queues of pending plans. */
  std::vector<PlanQueue> plans(params->flaw_orders.size(), PlanQueue());
  /* Dead plan queues. */
  std::vector<PlanQueue*> dead_queues;
  /* Construct the initial plan. */
  const Plan* initial_plan = make_initial_plan(problem);
  if (initial_plan != NULL) {
    initial_plan->id_ = 0;
  }

  /* Variable for progress bar (number of generated plans). */
  size_t last_dot = 0;
  /* Variable for progress bar (time). */
  size_t last_hash = 0;

  /*
   * Search for complete plan.
   */
  size_t current_flaw_order = 0;
  size_t flaw_orders_left = params->flaw_orders.size();
  size_t next_switch = 1000;
  const Plan* current_plan = initial_plan;
  generated_plans[current_flaw_order]++;
  num_generated_plans++;
  if (verbosity > 1) {
    std::cerr << "using flaw order " << current_flaw_order << std::endl;
  }
  float f_limit;
  if (current_plan != NULL
      && params->search_algorithm == Parameters::IDA_STAR) {
    f_limit = current_plan->primary_rank();
  } else {
    f_limit = INFINITY;
  }
  do {
    float next_f_limit = INFINITY;
    while (current_plan != NULL && !current_plan->complete()) {
      /* Do a little amortized cleanup of dead queues. */
      for (size_t dq = 0; dq < 4 && !dead_queues.empty(); dq++) {
	PlanQueue& dead_queue = *dead_queues.back();
	delete dead_queue.top();
	dead_queue.pop();
	if (dead_queue.empty()) {
	  dead_queues.pop_back();
	}
      }
      struct itimerval timer;
#ifdef PROFILING
      getitimer(ITIMER_VIRTUAL, &timer);
#else
      getitimer(ITIMER_PROF, &timer);
#endif
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      if (t >= 60.0*params->time_limit) {
	/* Time limit exceeded. */
	break;
      }

      /*
       * Visiting a new plan.
       */
      num_visited_plans++;
      if (verbosity == 1) {
	while (num_generated_plans - num_static - last_dot >= 1000) {
	  std::cerr << '.';
	  last_dot += 1000;
	}
	while (t - 60.0*last_hash >= 60.0) {
	  std::cerr << '#';
	  last_hash++;
	}
      }
      if (verbosity > 1) {
	std::cerr << std::endl << (num_visited_plans - num_static) << ": "
		  << "!!!!CURRENT PLAN (id " << current_plan->id_ << ")"
		  << " with rank (" << current_plan->primary_rank();
	for (size_t ri = 1; ri < current_plan->rank_.size(); ri++) {
	  std::cerr << ',' << current_plan->rank_[ri];
	}
	std::cerr << ")" << std::endl << *current_plan << std::endl;
      }
      /* List of children to current plan. */
      PlanList refinements;
      /* Get plan refinements. */
      current_plan->refinements(refinements,
				params->flaw_orders[current_flaw_order]);
      /* Add children to queue of pending plans. */
      bool added = false;
      for (PlanList::const_iterator pi = refinements.begin();
	   pi != refinements.end(); pi++) {
	const Plan& new_plan = **pi;
	/* N.B. Must set id before computing rank, because it may be used. */
	new_plan.id_ = num_generated_plans;
	if (!isinf(new_plan.primary_rank())
	    && (generated_plans[current_flaw_order]
		< params->search_limits[current_flaw_order])) {
	  if (params->search_algorithm == Parameters::IDA_STAR
	      && new_plan.primary_rank() > f_limit) {
	    next_f_limit = std::min(next_f_limit, new_plan.primary_rank());
	    delete &new_plan;
	    continue;
	  }
	  if (!added && static_pred_flaw) {
	    num_static++;
	  }
	  added = true;
	  plans[current_flaw_order].push(&new_plan);
	  generated_plans[current_flaw_order]++;
	  num_generated_plans++;
	  if (verbosity > 2) {
	    std::cerr << std::endl << "####CHILD (id " << new_plan.id_ << ")"
		      << " with rank (" << new_plan.primary_rank();
	    for (size_t ri = 1; ri < new_plan.rank_.size(); ri++) {
	      std::cerr << ',' << new_plan.rank_[ri];
	    }
	    std::cerr << "):" << std::endl << new_plan << std::endl;
	  }
	} else {
	  delete &new_plan;
	}
      }
      if (!added) {
	num_dead_ends++;
      }

      /*
       * Process next plan.
       */
      bool limit_reached = false;
      if ((limit_reached = (generated_plans[current_flaw_order]
			    >= params->search_limits[current_flaw_order]))
	  || generated_plans[current_flaw_order] >= next_switch) {
	if (verbosity > 1) {
	  std::cerr << "time to switch ("
		    << generated_plans[current_flaw_order] << ")" << std::endl;
	}
	if (limit_reached) {
	  flaw_orders_left--;
	  /* Discard the rest of the plan queue. */
	  dead_queues.push_back(&plans[current_flaw_order]);
	}
	if (flaw_orders_left > 0) {
	  do {
	    current_flaw_order++;
	    if (verbosity > 1) {
	      std::cerr << "use flaw order "
			<< current_flaw_order << "?" << std::endl;
	    }
	    if (current_flaw_order >= params->flaw_orders.size()) {
	      current_flaw_order = 0;
	      next_switch *= 2;
	    }
	  } while ((generated_plans[current_flaw_order]
		    >= params->search_limits[current_flaw_order]));
	  if (verbosity > 1) {
	    std::cerr << "using flaw order " << current_flaw_order
		      << std::endl;
	  }
	}
      }
      if (flaw_orders_left > 0) {
	if (generated_plans[current_flaw_order] == 0) {
	  current_plan = initial_plan;
	  generated_plans[current_flaw_order]++;
	  num_generated_plans++;
	} else {
	  if (current_plan != initial_plan) {
	    delete current_plan;
	  }
	  if (plans[current_flaw_order].empty()) {
	    /* Problem lacks solution. */
	    current_plan = NULL;
	  } else {
	    current_plan = plans[current_flaw_order].top();
	    plans[current_flaw_order].pop();
	  }
	}
	/*
	 * Instantiate all actions if the plan is otherwise complete.
	 */
	bool instantiated = params->ground_actions;
	while (current_plan != NULL && current_plan->complete()
	       && !instantiated) {
	  const Bindings* new_bindings =
	    step_instantiation(current_plan->steps(), 0,
			       *current_plan->bindings_);
	  if (new_bindings != NULL) {
	    instantiated = true;
	    if (new_bindings != current_plan->bindings_) {
	      const Plan* inst_plan =
		new Plan(current_plan->steps(), current_plan->num_steps(),
			 current_plan->links(), current_plan->num_links(),
			 current_plan->orderings(), *new_bindings,
			 NULL, 0, NULL, 0, current_plan);
	      delete current_plan;
	      current_plan = inst_plan;
	    }
	  } else if (plans[current_flaw_order].empty()) {
	    /* Problem lacks solution. */
	    current_plan = NULL;
	  } else {
	    current_plan = plans[current_flaw_order].top();
	    plans[current_flaw_order].pop();
	  }
	}
      } else {
	if (!isinf(next_f_limit)) {
	  current_plan = NULL;
	}
	break;
      }
    }
    if (current_plan != NULL && current_plan->complete()) {
      break;
    }
    f_limit = next_f_limit;
    if (!isinf(f_limit)) {
      /* Restart search. */
      if (current_plan != NULL && current_plan != initial_plan) {
	delete current_plan;
      }
      current_plan = initial_plan;
    }
  } while (!isinf(f_limit));
  if (verbosity > 0) {
    /*
     * Print statistics.
     */
    std::cerr << std::endl << "Plans generated: " << num_generated_plans;
    if (num_static > 0) {
      std::cerr << " [" << (num_generated_plans - num_static) << "]";
    }
    std::cerr << std::endl << "Plans visited: " << num_visited_plans;
    if (num_static > 0) {
      std::cerr << " [" << (num_visited_plans - num_static) << "]";
    }
    std::cerr << std::endl << "Dead ends encountered: " << num_dead_ends
	      << std::endl;
  }
  /*
   * Discard the rest of the plan queue and some other things, unless
   * this is the last problem in which case we can save time by just
   * letting the operating system reclaim the memory for us.
   */
  if (!last_problem) {
    if (current_plan != initial_plan) {
      delete initial_plan;
    }
    for (size_t i = 0; i < plans.size(); i++) {
      while (!plans[i].empty()) {
	delete plans[i].top();
	plans[i].pop();
      }
    }
  }
  /* Return last plan, or NULL if problem does not have a solution. */
  return current_plan;
}


/* Cleans up after planning. */
void Plan::cleanup() {
  if (planning_graph != NULL) {
    delete planning_graph;
    planning_graph = NULL;
  }
  if (goal_action != NULL) {
    delete goal_action;
    goal_action = NULL;
  }
}


/* Constructs a plan. */
Plan::Plan(const Chain<Step>* steps, size_t num_steps,
	   const Chain<Link>* links, size_t num_links,
	   const Orderings& orderings, const Bindings& bindings,
	   const Chain<Unsafe>* unsafes, size_t num_unsafes,
	   const Chain<OpenCondition>* open_conds, size_t num_open_conds,
	   const Plan* parent)
  : steps_(steps), num_steps_(num_steps),
    links_(links), num_links_(num_links),
    orderings_(&orderings), bindings_(&bindings),
    unsafes_(unsafes), num_unsafes_(num_unsafes),
    open_conds_(open_conds), num_open_conds_(num_open_conds) {
  Chain<Step>::register_use(steps);
  Chain<Link>::register_use(links);
  Orderings::register_use(&orderings);
  Bindings::register_use(&bindings);
  Chain<Unsafe>::register_use(unsafes);
  Chain<OpenCondition>::register_use(open_conds);
#ifdef DEBUG_MEMORY
  created_plans++;
#endif
#ifdef DEBUG
  depth_ = (parent != NULL) ? parent->depth() + 1 : 0;
#endif
}


/* Deletes this plan. */
Plan::~Plan() {
#ifdef DEBUG_MEMORY
  deleted_plans++;
#endif
  Chain<Step>::unregister_use(steps_);
  Chain<Link>::unregister_use(links_);
  Orderings::unregister_use(orderings_);
  Bindings::unregister_use(bindings_);
  Chain<Unsafe>::unregister_use(unsafes_);
  Chain<OpenCondition>::unregister_use(open_conds_);
}


/* Returns the bindings of this plan. */
const Bindings* Plan::bindings() const {
  return params->ground_actions ? NULL : bindings_;
}


/* Checks if this plan is complete. */
bool Plan::complete() const {
  return unsafes() == NULL && open_conds() == NULL;
}


/* Returns the primary rank of this plan, where a lower rank
   signifies a better plan. */
float Plan::primary_rank() const {
  if (rank_.empty()) {
    params->heuristic.plan_rank(rank_, *this, params->weight, *domain,
				planning_graph);
  }
  return rank_[0];
}


/* Returns the serial number of this plan. */
size_t Plan::serial_no() const {
  return id_;
}


/* Returns the next flaw to work on. */
const Flaw& Plan::get_flaw(const FlawSelectionOrder& flaw_order) const {
  const Flaw& flaw = flaw_order.select(*this, *problem, planning_graph);
  if (!params->ground_actions) {
    const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
    static_pred_flaw = (open_cond != NULL && open_cond->is_static(*domain));
  }
  return flaw;
}


/* Returns the refinements for the next flaw to work on. */
void Plan::refinements(PlanList& plans,
		       const FlawSelectionOrder& flaw_order) const {
  const Flaw& flaw = get_flaw(flaw_order);
  if (verbosity > 1) {
    std::cerr << std::endl << "handle ";
    flaw.print(std::cerr, domain->predicates(), problem->terms(), *bindings_);
    std::cerr << std::endl;
  }
  const Unsafe* unsafe = dynamic_cast<const Unsafe*>(&flaw);
  if (unsafe != NULL) {
    handle_unsafe(plans, *unsafe);
  } else {
    const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
    if (open_cond != NULL) {
      handle_open_condition(plans, *open_cond);
    } else {
      throw Exception("unknown kind of flaw");
    }
  }
}


/* Counts the number of refinements for the given threat, and returns
   true iff the number of refinements does not exceed the given
   limit. */
bool Plan::unsafe_refinements(int& refinements, int& separable,
			      int& promotable, int& demotable,
			      const Unsafe& unsafe, int limit) const {
  if (refinements >= 0) {
    return refinements <= limit;
  } else {
    int ref = 0;
    BindingList unifier;
    const Link& link = unsafe.link();
    StepTime lt1 = link.effect_time();
    StepTime lt2 = end_time(link.condition_time());
    StepTime et = end_time(unsafe.effect());
    if (((link.from_id() == unsafe.step_id() && lt1 == et)
	 || (orderings().possibly_before(link.from_id(), lt1,
					 unsafe.step_id(), et)
	     && orderings().possibly_after(link.to_id(), lt2,
					   unsafe.step_id(), et)))
	&& bindings_->affects(unifier, unsafe.effect_add(), unsafe.step_id(),
			      link.condition(), link.to_id(),
			      problem->domain().types(), problem->terms())) {
      PlanList dummy;
      if (separable < 0) {
	separable = separate(dummy, unsafe, unifier, true);
      }
      ref += separable;
      if (ref <= limit) {
	if (promotable < 0) {
	  promotable = promote(dummy, unsafe, true);
	}
	ref += promotable;
	if (ref <= limit) {
	  if (demotable < 0) {
	    demotable = demote(dummy, unsafe, true);
	  }
	  refinements = ref + demotable;
	  return refinements <= limit;
	}
      }
      return false;
    } else {
      separable = promotable = demotable = 0;
      refinements = 1;
      return refinements <= limit;
    }
  }
}


/* Handles an unsafe link. */
void Plan::handle_unsafe(PlanList& plans, const Unsafe& unsafe) const {
  BindingList unifier;
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition_time());
  StepTime et = end_time(unsafe.effect());
  if (((link.from_id() == unsafe.step_id() && lt1 == et)
       || (orderings().possibly_before(link.from_id(), lt1,
				       unsafe.step_id(), et)
	   && orderings().possibly_after(link.to_id(), lt2,
				       unsafe.step_id(), et)))
      && bindings_->affects(unifier, unsafe.effect_add(), unsafe.step_id(),
			    link.condition(), link.to_id(),
			    problem->domain().types(), problem->terms())) {
    separate(plans, unsafe, unifier);
    promote(plans, unsafe);
    demote(plans, unsafe);
  } else {
    /* bogus flaw */
    plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			     orderings(), *bindings_,
			     unsafes()->remove(unsafe), num_unsafes() - 1,
			     open_conds(), num_open_conds(), this));
  }
}


/* Checks if the given threat is separable. */
int Plan::separable(const Unsafe& unsafe) const {
  BindingList unifier;
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition_time());
  StepTime et = end_time(unsafe.effect());
  if (((link.from_id() == unsafe.step_id() && lt1 == et)
       || (orderings().possibly_before(link.from_id(), lt1,
				       unsafe.step_id(), et)
	   && orderings().possibly_after(link.to_id(), lt2,
				       unsafe.step_id(), et)))
      && bindings_->affects(unifier,
			    unsafe.effect_add(), unsafe.step_id(),
			    link.condition(), link.to_id(),
			    problem->domain().types(), problem->terms())) {
    PlanList dummy;
    return separate(dummy, unsafe, unifier, true);
  } else {
    return 0;
  }
}


/* Handles an unsafe link through separation. */
int Plan::separate(PlanList& plans, const Unsafe& unsafe,
		   const BindingList& unifier, bool test_only) const {
  const Condition* goal = &Condition::FALSE;
  for (BindingList::const_iterator si = unifier.begin();
       si != unifier.end(); si++) {
    const Binding& subst = *si;
    if (!unsafe.effect().quantifies(subst.var())) {
      const Inequality& neq = *new Inequality(subst.var(), subst.var_id(),
					      subst.term(), subst.term_id());
      if (bindings_->consistent_with(neq, 0)) {
	goal = &(*goal || Condition::make(neq, AT_START));
      } else {
	Formula::register_use(&neq);
	Formula::unregister_use(&neq);
      }
    }
  }
  const Condition& effect_cond = unsafe.effect().condition();
  if (!effect_cond.tautology()) {
    size_t n = unsafe.effect().arity();
    if (n > 0) {
      Forall* forall_s = new Forall();
      Forall* forall_i = new Forall();
      Forall* forall_e = new Forall();
      SubstitutionMap forall_subst;
      for (size_t i = 0; i < n; i++) {
	Variable vi = unsafe.effect().parameter(i);
	Variable v =
	  test_only ? vi : problem->new_variable(problem->terms().type(vi));
	forall_s->add_parameter(v);
	forall_i->add_parameter(v);
	forall_e->add_parameter(v);
	if (!test_only) {
	  forall_subst.insert(std::make_pair(vi, v));
	}
      }
      if (test_only) {
	forall_s->set_body(!effect_cond.at_start());
	forall_i->set_body(!effect_cond.over_all());
	forall_e->set_body(!effect_cond.at_end());
      } else {
	forall_s->set_body(!effect_cond.at_start().substitution(forall_subst));
	forall_i->set_body(!effect_cond.over_all().substitution(forall_subst));
	forall_e->set_body(!effect_cond.at_end().substitution(forall_subst));
      }
      const Formula* at_start;
      if (forall_s->body().tautology() || forall_s->body().contradiction()) {
	at_start = &forall_s->body();
	delete forall_s;
      } else {
	at_start = forall_s;
      }
      const Formula* over_all;
      if (forall_i->body().tautology() || forall_i->body().contradiction()) {
	over_all = &forall_i->body();
	delete forall_i;
      } else {
	over_all = forall_i;
      }
      const Formula* at_end;
      if (forall_e->body().tautology() || forall_e->body().contradiction()) {
	at_end = &forall_e->body();
	delete forall_e;
      } else {
	at_end = forall_e;
      }
      goal = &(*goal || Condition::make(*at_start, *over_all, *at_end));
    } else {
      goal = &(*goal || !effect_cond);
    }
  }
  const Chain<OpenCondition>* new_open_conds = test_only ? NULL : open_conds();
  size_t new_num_open_conds = test_only ? 0 : num_open_conds();
  BindingList new_bindings;
  bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			*goal, unsafe.step_id(), test_only);
  if (!test_only) {
    Chain<OpenCondition>::register_use(new_open_conds);
  }
  int count = 0;
  if (added) {
    const Bindings* bindings = bindings_->add(new_bindings,
					      problem->domain().types(),
					      problem->terms(), test_only);
    if (bindings != NULL) {
      if (!test_only) {
	const Orderings* new_orderings = orderings_;
	if (!goal->tautology()) {
	  const TemporalOrderings* to =
	    dynamic_cast<const TemporalOrderings*>(new_orderings);
	  if (to != NULL) {
	    HeuristicValue h, hs;
	    goal->heuristic_value(h, hs, *planning_graph, unsafe.step_id(),
				  params->ground_actions ? NULL : bindings);
	    new_orderings = to->refine(unsafe.step_id(),
				       hs.makespan(), h.makespan());
	  }
	}
	if (new_orderings != NULL) {
	  plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
				   *new_orderings, *bindings,
				   unsafes()->remove(unsafe),
				   num_unsafes() - 1,
				   new_open_conds, new_num_open_conds, this));
	} else {
	  Bindings::register_use(bindings);
	  Bindings::unregister_use(bindings);
	}
      }
      count++;
    }
  }
  if (!test_only) {
    Chain<OpenCondition>::unregister_use(new_open_conds);
  }
  Condition::register_use(goal);
  Condition::unregister_use(goal);
  return count;
}


/* Handles an unsafe link through demotion. */
int Plan::demote(PlanList& plans, const Unsafe& unsafe,
		 bool test_only) const {
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime et = end_time(unsafe.effect());
  if (orderings().possibly_before(unsafe.step_id(), et, link.from_id(), lt1)) {
    if (!test_only) {
      new_ordering(plans, unsafe.step_id(), et, link.from_id(), lt1, unsafe);
    }
    return 1;
  } else {
    return 0;
  }
}


/* Handles an unsafe link through promotion. */
int Plan::promote(PlanList& plans, const Unsafe& unsafe,
		  bool test_only) const {
  const Link& link = unsafe.link();
  StepTime lt2 = end_time(link.condition_time());
  StepTime et = end_time(unsafe.effect());
  if (orderings().possibly_before(link.to_id(), lt2, unsafe.step_id(), et)) {
    if (!test_only) {
      new_ordering(plans, link.to_id(), lt2, unsafe.step_id(), et, unsafe);
    }
    return 1;
  } else {
    return 0;
  }
}


/* Adds a plan to the given plan list with an ordering added. */
void Plan::new_ordering(PlanList& plans, size_t before_id, StepTime t1,
			size_t after_id, StepTime t2,
			const Unsafe& unsafe) const {
  const Orderings* new_orderings =
    orderings().refine(Ordering(before_id, t1, after_id, t2));
  if (new_orderings != NULL) {
    plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			     *new_orderings, *bindings_,
			     unsafes()->remove(unsafe), num_unsafes() - 1,
			     open_conds(), num_open_conds(), this));
  }
}


/* Checks if the given open conditions is threatened. */
bool Plan::unsafe_open_condition(const OpenCondition& open_cond) const {
  const Literal* literal = open_cond.literal();
  if (literal != NULL) {
    const Literal& goal = *literal;
    StepTime gt = end_time(open_cond.when());
    for (const Chain<Step>* sc = steps(); sc != NULL; sc = sc->tail) {
      const Step& s = sc->head;
      if (orderings().possibly_before(s.id(), STEP_START,
				      open_cond.step_id(), gt)) {
	const EffectList& effects = s.action().effects();
	for (EffectList::const_iterator ei = effects.begin();
	     ei != effects.end(); ei++) {
	  const Effect& e = **ei;
	  StepTime et = end_time(e);
	  if (orderings().possibly_before(s.id(), et,
					  open_cond.step_id(), gt)) {
	    if (bindings_->affects(e.literal(), s.id(),
				   goal, open_cond.step_id(),
				   problem->domain().types(),
				   problem->terms())) {
	      return true;
	    }
	  }
	}
      }
    }
  }
  return false;
}


/* Counts the number of refinements for the given open condition, and
   returns true iff the number of refinements does not exceed the
   given limit. */
bool Plan::open_cond_refinements(int& refinements, int& addable, int& reusable,
				 const OpenCondition& open_cond,
				 int limit) const {
  if (refinements >= 0) {
    return refinements <= limit;
  } else {
    const Literal* literal = open_cond.literal();
    if (literal != NULL) {
      int ref = 0;
      if (addable < 0) {
	if (!addable_steps(addable, *literal, open_cond, limit)) {
	  return false;
	}
      }
      ref += addable;
      if (ref <= limit) {
	if (reusable < 0) {
	  if (!reusable_steps(reusable, *literal, open_cond, limit)) {
	    return false;
	  }
	}
	refinements = ref + reusable;
	return refinements <= limit;
      }
    } else {
      PlanList dummy;
      const Disjunction* disj = open_cond.disjunction();
      if (disj != NULL) {
	refinements = handle_disjunction(dummy, *disj, open_cond, true);
	return refinements <= limit;
      } else {
	const Inequality* neq = open_cond.inequality();
	if (neq != NULL) {
	  refinements = handle_inequality(dummy, *neq, open_cond, true);
	} else {
	  throw Exception("unknown kind of open condition");
	}
      }
    }
  }
  return false;
}


/* Handles an open condition. */
void Plan::handle_open_condition(PlanList& plans,
				 const OpenCondition& open_cond) const {
  const Literal* literal = open_cond.literal();
  if (literal != NULL) {
    const ActionEffectMap* achievers = literal_achievers(*literal);
    if (achievers != NULL) {
      add_step(plans, *literal, open_cond, *achievers);
      reuse_step(plans, *literal, open_cond, *achievers);
    }
    const Negation* negation = dynamic_cast<const Negation*>(literal);
    if (negation != NULL) {
      new_cw_link(plans, problem->init_action().effects(),
		  *negation, open_cond);
    }
  } else {
    const Disjunction* disj = open_cond.disjunction();
    if (disj != NULL) {
      handle_disjunction(plans, *disj, open_cond);
    } else {
      const Inequality* neq = open_cond.inequality();
      if (neq != NULL) {
	handle_inequality(plans, *neq, open_cond);
      } else {
	throw Exception("unknown kind of open condition");
      }
    }
  }
}


/* Handles a disjunctive open condition. */
int Plan::handle_disjunction(PlanList& plans, const Disjunction& disj,
			     const OpenCondition& open_cond,
			     bool test_only) const {
  int count = 0;
  const FormulaList& disjuncts = disj.disjuncts();
  for (FormulaList::const_iterator fi = disjuncts.begin();
       fi != disjuncts.end(); fi++) {
    BindingList new_bindings;
    const Chain<OpenCondition>* new_open_conds =
      test_only ? NULL : open_conds()->remove(open_cond);
    size_t new_num_open_conds = test_only ? 0 : num_open_conds() - 1;
    bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			  **fi, open_cond.when(), open_cond.step_id(),
			  test_only);
    if (!test_only) {
      Chain<OpenCondition>::register_use(new_open_conds);
    }
    if (added) {
      const Bindings* bindings = bindings_->add(new_bindings,
						problem->domain().types(),
						problem->terms(), test_only);
      if (bindings != NULL) {
	if (!test_only) {
	  plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
				   orderings(), *bindings,
				   unsafes(), num_unsafes(),
				   new_open_conds, new_num_open_conds, this));
	}
	count++;
      }
    }
    if (!test_only) {
      Chain<OpenCondition>::unregister_use(new_open_conds);
    }
  }
  return count;
}


/* Handles inequality open condition. */
int Plan::handle_inequality(PlanList& plans, const Inequality& neq,
			    const OpenCondition& open_cond,
			    bool test_only) const {
  int count = 0;
  size_t step_id = open_cond.step_id();
  const NameSet& d1 = bindings_->domain(neq.term1(), neq.step_id1(step_id),
					*problem);
  const NameSet& d2 = bindings_->domain(neq.term2(), neq.step_id2(step_id),
					*problem);

  /*
   * Branch on the variable with the smallest domain.
   */
  Variable var1;
  size_t id1;
  Variable var2;
  size_t id2;
  const NameSet* var_domain;
  if (d1.size() < d2.size()) {
    var1 = neq.term1();
    id1 = neq.step_id1(step_id);
    var2 = neq.term2();
    id2 = neq.step_id2(step_id);
    var_domain = &d1;
  } else {
    var1 = neq.term2();
    id1 = neq.step_id2(step_id);
    var2 = neq.term1();
    id2 = neq.step_id1(step_id);
    var_domain = &d2;
  }
  for (NameSet::const_iterator ni = var_domain->begin();
       ni != var_domain->end(); ni++) {
    Object name = *ni;
    BindingList new_bindings;
    new_bindings.push_back(Binding(var1, id1, name, 0, true));
    new_bindings.push_back(Binding(var2, id2, name, 0, false));
    const Bindings* bindings = bindings_->add(new_bindings,
					      problem->domain().types(),
					      problem->terms(), test_only);
    if (bindings != NULL) {
      if (!test_only) {
	plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
				 orderings(), *bindings,
				 unsafes(), num_unsafes(),
				 open_conds()->remove(open_cond),
				 num_open_conds() - 1, this));
      }
      count++;
    }
  }
  if (planning_graph == NULL) {
    delete &d1;
    delete &d2;
  }
  return count;
}


/* Counts the number of add-step refinements for the given literal
   open condition, and returns true iff the number of refinements
   does not exceed the given limit. */
bool Plan::addable_steps(int& refinements, const Literal& literal,
			 const OpenCondition& open_cond, int limit) const {
  int count = 0;
  PlanList dummy;
  const ActionEffectMap* achievers = literal_achievers(literal);
  if (achievers != NULL) {
    for (ActionEffectMap::const_iterator ai = achievers->begin();
	 ai != achievers->end(); ai++) {
      const Action& action = *(*ai).first;
      if (action.name().substr(0, 1) != "<") {
	const Effect& effect = *(*ai).second;
	count += new_link(dummy, Step(num_steps() + 1, action), effect,
			  literal, open_cond, true);
	if (count > limit) {
	  return false;
	}
      }
    }
  }
  refinements = count;
  return count <= limit;
}


/* Handles a literal open condition by adding a new step. */
void Plan::add_step(PlanList& plans, const Literal& literal,
		    const OpenCondition& open_cond,
		    const ActionEffectMap& achievers) const {
  for (ActionEffectMap::const_iterator ai = achievers.begin();
       ai != achievers.end(); ai++) {
    const Action& action = *(*ai).first;
    if (action.name().substr(0, 1) != "<") {
      const Effect& effect = *(*ai).second;
      new_link(plans, Step(num_steps() + 1, action), effect,
	       literal, open_cond);
    }
  }
}


/* Counts the number of reuse-step refinements for the given literal
   open condition, and returns true iff the number of refinements
   does not exceed the given limit. */
bool Plan::reusable_steps(int& refinements, const Literal& literal,
			  const OpenCondition& open_cond, int limit) const {
  int count = 0;
  PlanList dummy;
  const ActionEffectMap* achievers = literal_achievers(literal);
  if (achievers != NULL) {
    StepTime gt = start_time(open_cond.when());
    for (const Chain<Step>* sc = steps(); sc != NULL; sc = sc->tail) {
      const Step& step = sc->head;
      if (orderings().possibly_before(step.id(), STEP_START,
				      open_cond.step_id(), gt)) {
	std::pair<ActionEffectMap::const_iterator,
	  ActionEffectMap::const_iterator> b =
	  achievers->equal_range(&step.action());
	for (ActionEffectMap::const_iterator ei = b.first;
	     ei != b.second; ei++) {
	  const Effect& effect = *(*ei).second;
	  StepTime et = end_time(effect);
	  if (orderings().possibly_before(step.id(), et,
					  open_cond.step_id(), gt)) {
	    count += new_link(dummy, step, effect, literal, open_cond, true);
	    if (count > limit) {
	      return false;
	    }
	  }
	}
      }
    }
  }
  const Negation* negation = dynamic_cast<const Negation*>(&literal);
  if (negation != NULL) {
    count += new_cw_link(dummy, problem->init_action().effects(),
			 *negation, open_cond, true);
  }
  refinements = count;
  return count <= limit;
}


/* Handles a literal open condition by reusing an existing step. */
void Plan::reuse_step(PlanList& plans, const Literal& literal,
		      const OpenCondition& open_cond,
		      const ActionEffectMap& achievers) const {
  StepTime gt = start_time(open_cond.when());
  for (const Chain<Step>* sc = steps(); sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (orderings().possibly_before(step.id(), STEP_START,
				    open_cond.step_id(), gt)) {
      std::pair<ActionEffectMap::const_iterator,
	ActionEffectMap::const_iterator> b =
	achievers.equal_range(&step.action());
      for (ActionEffectMap::const_iterator ei = b.first;
	   ei != b.second; ei++) {
	const Effect& effect = *(*ei).second;
	StepTime et = end_time(effect);
	if (orderings().possibly_before(step.id(), et,
					open_cond.step_id(), gt)) {
	  new_link(plans, step, effect, literal, open_cond);
	}
      }
    }
  }
}


/* Adds plans to the given plan list with a link from the given step
   to the given open condition added. */
int Plan::new_link(PlanList& plans, const Step& step, const Effect& effect,
		   const Literal& literal, const OpenCondition& open_cond,
		   bool test_only) const {
  BindingList mgu;
  if (bindings_->unify(mgu, literal, open_cond.step_id(),
		       effect.literal(), step.id(),
		       problem->domain().types(), problem->terms())) {
    return make_link(plans, step, effect, literal, open_cond, mgu, test_only);
  } else {
    return 0;
  }
}


/* Adds plans to the given plan list with a link from the given step
   to the given open condition added using the closed world
   assumption. */
int Plan::new_cw_link(PlanList& plans, const EffectList& effects,
		      const Negation& negation, const OpenCondition& open_cond,
		      bool test_only) const {
  const Atom& goal = negation.atom();
  const Formula* goals = &Formula::TRUE;
  for (EffectList::const_iterator ei = effects.begin();
       ei != effects.end(); ei++) {
    const Effect& effect = **ei;
    BindingList mgu;
    if (bindings_->unify(mgu, goal, open_cond.step_id(),
			 effect.literal(), 0,
			 problem->domain().types(), problem->terms())) {
      if (mgu.empty()) {
	/* Impossible to separate goal and initial condition. */
	return 0;
      }
      const Formula* binds = &Formula::FALSE;
      for (BindingList::const_iterator si = mgu.begin();
	   si != mgu.end(); si++) {
	const Binding& subst = *si;
	binds = &(*binds || Inequality::make(subst.var(), subst.var_id(),
					     subst.term(), subst.term_id()));
      }
      goals = &(*goals && *binds);
    }
  }
  BindingList new_bindings;
  const Chain<OpenCondition>* new_open_conds =
    test_only ? NULL : open_conds()->remove(open_cond);
  size_t new_num_open_conds = test_only ? 0 : num_open_conds() - 1;
  bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			*goals, AT_START, 0, test_only);
  Formula::register_use(goals);
  Formula::unregister_use(goals);
  if (!test_only) {
    Chain<OpenCondition>::register_use(new_open_conds);
  }
  int count = 0;
  if (added) {
    const Bindings* bindings = bindings_->add(new_bindings,
					      problem->domain().types(),
					      problem->terms(), test_only);
    if (bindings != NULL) {
      if (!test_only) {
	const Chain<Unsafe>* new_unsafes = unsafes();
	size_t new_num_unsafes = num_unsafes();
	const Chain<Link>* new_links =
	  new Chain<Link>(Link(0, STEP_END, open_cond), links());
	link_threats(new_unsafes, new_num_unsafes, new_links->head, steps(),
		     orderings(), *bindings);
	plans.push_back(new Plan(steps(), num_steps(),
				 new_links, num_links() + 1,
				 orderings(), *bindings,
				 new_unsafes, new_num_unsafes,
				 new_open_conds, new_num_open_conds, this));
      }
      count++;
    }
  }
  if (!test_only) {
    Chain<OpenCondition>::unregister_use(new_open_conds);
  }
  return count;
}


/* Returns a plan with a link added from the given effect to the
   given open condition. */
int Plan::make_link(PlanList& plans, const Step& step, const Effect& effect,
		    const Literal& literal, const OpenCondition& open_cond,
		    const BindingList& unifier, bool test_only) const {
  /*
   * Add bindings needed to unify effect and goal.
   */
  BindingList new_bindings;
  SubstitutionMap forall_subst;
  if (test_only) {
    new_bindings = unifier;
  } else {
    for (BindingList::const_iterator si = unifier.begin();
	 si != unifier.end(); si++) {
      const Binding& subst = *si;
      if (effect.quantifies(subst.var())) {
	Variable v = problem->new_variable(problem->terms().type(subst.var()));
	forall_subst.insert(std::make_pair(subst.var(), v));
	new_bindings.push_back(Binding(v, subst.var_id(),
				       subst.term(), subst.term_id(), true));
      } else {
	new_bindings.push_back(subst);
      }
    }
  }

  /*
   * If the effect is conditional, add condition as goal.
   */
  const Chain<OpenCondition>* new_open_conds =
    test_only ? NULL : open_conds()->remove(open_cond);
  size_t new_num_open_conds = test_only ? 0 : num_open_conds() - 1;
  const Condition* cond_goal =
    &(effect.condition() && effect.link_condition());
  if (!cond_goal->tautology()) {
    if (!test_only) {
      size_t n = effect.arity();
      if (n > 0) {
	for (size_t i = 0; i < n; i++) {
	  Variable vi = effect.parameter(i);
	  if (forall_subst.find(vi) == forall_subst.end()) {
	    Variable v = problem->new_variable(problem->terms().type(vi));
	    forall_subst.insert(std::make_pair(vi, v));
	  }
	}
	const Condition* old_cond_goal = cond_goal;
	cond_goal = &cond_goal->substitution(forall_subst);
	if (old_cond_goal != cond_goal) {
	  Condition::register_use(old_cond_goal);
	  Condition::unregister_use(old_cond_goal);
	}
      }
    }
    bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			  *cond_goal, step.id(), test_only);
    Condition::register_use(cond_goal);
    Condition::unregister_use(cond_goal);
    if (!added) {
      if (!test_only) {
	Chain<OpenCondition>::register_use(new_open_conds);
	Chain<OpenCondition>::unregister_use(new_open_conds);
      }
      return 0;
    }
  }

  /*
   * See if this is a new step.
   */
  const Bindings* bindings = bindings_;
  const Chain<Step>* new_steps = test_only ? NULL : steps();
  size_t new_num_steps = test_only ? 0 : num_steps();
  if (step.id() > num_steps()) {
    if (!add_goal(new_open_conds, new_num_open_conds, new_bindings,
		  step.action().condition(), step.id(), test_only)) {
      if (!test_only) {
	Chain<OpenCondition>::register_use(new_open_conds);
	Chain<OpenCondition>::unregister_use(new_open_conds);
      }
      return 0;
    }
    if (params->domain_constraints) {
      bindings = bindings->add(step.id(), step.action(), *planning_graph);
      if (bindings == NULL) {
	if (!test_only) {
	  Chain<OpenCondition>::register_use(new_open_conds);
	  Chain<OpenCondition>::unregister_use(new_open_conds);
	}
	return 0;
      }
    }
    if (!test_only) {
      new_steps = new Chain<Step>(step, new_steps);
      new_num_steps++;
    }
  }
  const Bindings* tmp_bindings = bindings->add(new_bindings,
					       problem->domain().types(),
					       problem->terms(), test_only);
  if ((test_only || tmp_bindings != bindings) && bindings != bindings_) {
    delete bindings;
  }
  if (tmp_bindings == NULL) {
    if (!test_only) {
      Chain<OpenCondition>::register_use(new_open_conds);
      Chain<OpenCondition>::unregister_use(new_open_conds);
      Chain<Step>::register_use(new_steps);
      Chain<Step>::unregister_use(new_steps);
      return 0;
    }
  }
  if (!test_only) {
    bindings = tmp_bindings;
    StepTime et = end_time(effect);
    StepTime gt = start_time(open_cond.when());
    const Orderings* new_orderings =
      orderings().refine(Ordering(step.id(), et, open_cond.step_id(), gt),
			 step, planning_graph,
			 params->ground_actions ? NULL : bindings);
    if (new_orderings != NULL && !cond_goal->tautology()) {
      const TemporalOrderings* to =
	dynamic_cast<const TemporalOrderings*>(new_orderings);
      if (to != NULL) {
	HeuristicValue h, hs;
	cond_goal->heuristic_value(h, hs, *planning_graph, step.id(),
				   params->ground_actions ? NULL : bindings);
	const Orderings* tmp_orderings = to->refine(step.id(), hs.makespan(),
						    h.makespan());
	if (tmp_orderings != new_orderings) {
	  delete new_orderings;
	  new_orderings = tmp_orderings;
	}
      }
    }
    if (new_orderings == NULL) {
      if (bindings != bindings_) {
	delete bindings;
      }
      Chain<OpenCondition>::register_use(new_open_conds);
      Chain<OpenCondition>::unregister_use(new_open_conds);
      Chain<Step>::register_use(new_steps);
      Chain<Step>::unregister_use(new_steps);
      return 0;
    }

    /*
     * Add a new link.
     */
    const Chain<Link>* new_links =
      new Chain<Link>(Link(step.id(), end_time(effect), open_cond), links());

    /*
     * Find any threats to the newly established link.
     */
    const Chain<Unsafe>* new_unsafes = unsafes();
    size_t new_num_unsafes = num_unsafes();
    link_threats(new_unsafes, new_num_unsafes, new_links->head, new_steps,
		 *new_orderings, *bindings);

    /*
     * If this is a new step, find links it threatens.
     */
    if (step.id() > num_steps()) {
      step_threats(new_unsafes, new_num_unsafes, step, links(), *new_orderings,
		   *bindings);
    }

    /* Adds the new plan. */
    plans.push_back(new Plan(new_steps, new_num_steps, new_links,
			     num_links() + 1, *new_orderings, *bindings,
			     new_unsafes, new_num_unsafes,
			     new_open_conds, new_num_open_conds, this));
  }
  return 1;
}


/* Less than operator for plans. */
bool operator<(const Plan& p1, const Plan& p2) {
  float diff = p1.primary_rank() - p2.primary_rank();
  for (size_t i = 1; i < p1.rank_.size() && diff == 0.0; i++) {
    diff = p1.rank_[i] - p2.rank_[i];
  }
  return diff > 0.0;
}


/*
 * Sorting of steps based on distance from initial conditions.
 */
struct StepSorter {
  StepSorter(std::map<size_t, float>& dist)
    : dist(dist) {}

  bool operator()(const Step* s1, const Step* s2) const {
    return dist[s1->id()] < dist[s2->id()];
  }

  std::map<size_t, float>& dist;
};


/* Find interfering steps. */
static const Orderings&
disable_interference(const std::vector<const Step*>& ordered_steps,
		     const Chain<Link>* links,
		     const BinaryOrderings& orderings,
		     const Bindings& bindings) {
  const BinaryOrderings* new_orderings = &orderings;
  size_t n = ordered_steps.size();
  for (size_t i = 0; i < n; i++) {
    for (size_t j = i + 1; j < n; j++) {
      const Step& si = *ordered_steps[i];
      const Step& sj = *ordered_steps[j];
      if (new_orderings->possibly_concurrent(si.id(), STEP_START,
					     sj.id(), STEP_START)
	  || new_orderings->possibly_concurrent(si.id(), STEP_START,
						sj.id(), STEP_END)
	  || new_orderings->possibly_concurrent(si.id(), STEP_END,
						sj.id(), STEP_START)
	  || new_orderings->possibly_concurrent(si.id(), STEP_END,
						sj.id(), STEP_END)) {
	if (verbosity > 1) {
	  std::cerr << si.id() << " and " << sj.id() << " possibly concurrent"
		    << std::endl;
	}
	bool interference = false;
	for (const Chain<Link>* lc = links;
	     lc != NULL && !interference; lc = lc->tail) {
	  const Link& l = lc->head;
	  StepTime lt = end_time(l.condition_time());
	  if (l.to_id() == sj.id()) {
	    // is effect of si interfering with link condition?
	    const EffectList& effects = si.action().effects();
	    for (EffectList::const_iterator ei = effects.begin();
		 ei != effects.end() && !interference; ei++) {
	      const Effect& e = **ei;
	      if (e.link_condition().contradiction()) {
		// effect could interfere with condition
		StepTime et = end_time(e);
		if (new_orderings->possibly_concurrent(si.id(), et,
						       l.to_id(), lt)) {
		  if (bindings.affects(e.literal(), si.id(),
				       l.condition(), l.to_id(),
				       problem->domain().types(),
				       problem->terms())) {
		    interference = true;
		  }
		}
	      }
	    }
	  } else if (l.to_id() == si.id()) {
	    // is effect of sj interfering with link condition?
	    const EffectList& effects = sj.action().effects();
	    for (EffectList::const_iterator ei = effects.begin();
		 ei != effects.end() && !interference; ei++) {
	      const Effect& e = **ei;
	      if (e.link_condition().contradiction()) {
		// effect could interfere with condition
		StepTime et = end_time(e);
		if (new_orderings->possibly_concurrent(sj.id(), et,
						       l.to_id(), lt)) {
		  if (bindings.affects(e.literal(), sj.id(),
				       l.condition(), l.to_id(),
				       problem->domain().types(),
				       problem->terms())) {
		    interference = true;
		  }
		}
	      }
	    }
	  }
	}
	if (interference) {
	  if (verbosity > 1) {
	    std::cerr << si.id() << " and " << sj.id() << " interfering"
		      << std::endl;
	  }
	  const Orderings* old_orderings = new_orderings;
	  new_orderings = new_orderings->refine(Ordering(si.id(), STEP_START,
							 sj.id(), STEP_START));
	  if (old_orderings != new_orderings) {
	    Orderings::register_use(old_orderings);
	    Orderings::unregister_use(old_orderings);
	  }
	}
      }
    }
  }
  return *new_orderings;
}


/* Output operator for plans. */
std::ostream& operator<<(std::ostream& os, const Plan& p) {
  const Step* init = NULL;
  const Step* goal = NULL;
  const Bindings* bindings = p.bindings_;
  std::vector<const Step*> ordered_steps;
  for (const Chain<Step>* sc = p.steps(); sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (step.id() == 0) {
      init = &step;
    } else if (step.id() == Plan::GOAL_ID) {
      goal = &step;
    } else {
      ordered_steps.push_back(&step);
    }
  }
  std::map<size_t, float> start_times;
  std::map<size_t, float> end_times;
  float makespan = p.orderings().schedule(start_times, end_times);
  sort(ordered_steps.begin(), ordered_steps.end(), StepSorter(start_times));
  /*
   * Now make sure that nothing scheduled at the same time is
   * interfering.  This can only occur if there are link conditions on
   * effects, and we can always resolve the threats by arbitrarily
   * ordering the interfering steps.  Note, however, that this is not
   * necessarily the if there are durative actions.  Because of this
   * we never add link conditions when the `:durative-actions'
   * requirement flag is present in the domain description, so we
   * don't have to worry about it here.
   */
  const BinaryOrderings* orderings =
    dynamic_cast<const BinaryOrderings*>(&p.orderings());
  if (p.complete() && orderings != NULL) {
    const Orderings& new_orderings =
      disable_interference(ordered_steps, p.links(),
			   *orderings, *p.bindings_);
    if (&new_orderings != &p.orderings()) {
      start_times.clear();
      end_times.clear();
      makespan = new_orderings.schedule(start_times, end_times);
      sort(ordered_steps.begin(), ordered_steps.end(),
	   StepSorter(start_times));
      Orderings::register_use(&new_orderings);
      Orderings::unregister_use(&new_orderings);
    }
  }
  if (verbosity < 2) {
    std::cerr << "Makespan: " << makespan << std::endl;
    bool first = true;
    for (std::vector<const Step*>::const_iterator si = ordered_steps.begin();
	 si != ordered_steps.end(); si++) {
      const Step& s = **si;
      if (s.action().name().substr(0, 1) != "<") {
	if (verbosity > 0 || !first) {
	  os << std::endl;
	}
	first = false;
	os << start_times[s.id()] << ':';
	s.action().print(os, problem->terms(), s.id(), *bindings);
	if (s.action().durative()) {
	  os << '[' << (end_times[s.id()] - start_times[s.id()]) << ']';
	}
      }
    }
  } else {
    os << "Initial  :";
    const EffectList& effects = init->action().effects();
    for (EffectList::const_iterator ei = effects.begin();
	 ei != effects.end(); ei++) {
      os << ' ';
      (*ei)->literal().print(os, problem->domain().predicates(),
			     problem->terms(), 0, *bindings);
    }
    ordered_steps.push_back(goal);
    for (std::vector<const Step*>::const_iterator si = ordered_steps.begin();
	 si != ordered_steps.end(); si++) {
      const Step& step = **si;
      if (step.id() == Plan::GOAL_ID) {
	os << std::endl << std::endl << "Goal     : ";
      } else {
	os << std::endl << std::endl << "Step " << step.id();
	if (step.id() < 100) {
	  if (step.id() < 10) {
	    os << ' ';
	  }
	  os << ' ';
	}
	os << " : ";
	step.action().print(os, problem->terms(), step.id(), *bindings);
      }
      for (const Chain<Link>* lc = p.links(); lc != NULL; lc = lc->tail) {
	const Link& link = lc->head;
	if (link.to_id() == step.id()) {
	  os << std::endl << "          " << link.from_id();
	  if (link.from_id() < 100) {
	    if (link.from_id() < 10) {
	      os << ' ';
	    }
	    os << ' ';
	  }
	  os << " -> (";
	  switch (link.condition_time()) {
	  case AT_START:
	    os << "at start ";
	    break;
	  case OVER_ALL:
	    os << "over all ";
	    break;
	  case AT_END:
	    os << "at end ";
	    break;
	  }
	  link.condition().print(os, problem->domain().predicates(),
				 problem->terms(), link.to_id(), *bindings);
	  os << ")";
	  for (const Chain<Unsafe>* uc = p.unsafes();
	       uc != NULL; uc = uc->tail) {
	    const Unsafe& unsafe = uc->head;
	    if (unsafe.link() == link) {
	      os << " <" << unsafe.step_id() << ">";
	    }
	  }
	}
      }
      for (const Chain<OpenCondition>* occ = p.open_conds();
	   occ != NULL; occ = occ->tail) {
	const OpenCondition& open_cond = occ->head;
	if (open_cond.step_id() == step.id()) {
	  os << std::endl << "           ?? -> (";
	  switch (open_cond.when()) {
	  case AT_START:
	    os << "at start ";
	    break;
	  case OVER_ALL:
	    os << "over all ";
	    break;
	  case AT_END:
	    os << "at end ";
	    break;
	  }
	  open_cond.condition().print(os, problem->domain().predicates(),
				      problem->terms(), open_cond.step_id(),
				      *bindings);
	  os << ")";
	}
      }
    }
    os << std::endl << "orderings = " << p.orderings();
    if (p.bindings() != NULL) {
      os << std::endl << "bindings = ";
      bindings->print(os, problem->terms());
    }
  }
  return os;
}
